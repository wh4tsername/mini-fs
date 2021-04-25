#include "handlers.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include <helpers.h>
#include <constants/fs_constants.h>

#include <stdbool.h>

bool check_if_obj_exists(int fd,
                         struct inode* inode,
                         uint16_t block_id,
                         const char* name,
                         uint16_t* res_inode_id) {
    struct dir_record records[NUM_RECORDS_IN_DIR];
    read_dir_records(fd, block_id, records, inode->size);

    for (uint16_t i = 0; i < inode->size; ++i) {
        if (strcmp(name, records[i].name) == 0) {
            *res_inode_id = records[i].inode_id;
            return true;
        }
    }

    return false;
}

// still need to write superblock
void delete_file(int fd,
                 struct superblock* sb,
                 struct inode* inode,
                 uint16_t inode_id) {
    // delete children blocks
    for (uint16_t i = 0; i < NUM_BLOCK_IDS_IN_INODE; ++i) {
        if (inode->block_ids[i] == 0) {
            break;
        }

        free_block(sb, inode->block_ids[i]);
        erase_block(fd, inode->block_ids[i]);
    }

    // delete child inode
    if (inode->inode_id != 0) {
        struct inode next_inode;
        reset_inode(&next_inode);
        read_inode(fd, inode->inode_id, &next_inode);

        delete_file(fd, sb, &next_inode, inode->inode_id);
    }

    // delete myself
    free_inode(sb, inode_id);
    erase_inode(fd, inode_id);
}

// still need to write superblock
void delete_directory(int fd,
                      struct superblock* sb,
                      struct inode* inode,
                      uint16_t inode_id) {
    uint16_t block_id = inode->block_ids[0];

    struct dir_record records[inode->size];
    read_dir_records(fd, block_id, records, inode->size);

    // delete children
    for (uint16_t i = 2; i < inode->size; ++i) {
        struct inode child_inode;
        reset_inode(&child_inode);

        read_inode(fd, records[i].inode_id, &child_inode);

        if (child_inode.is_file) {
            delete_file(fd, sb, &child_inode, records[i].inode_id);
        } else {
            delete_directory(fd, sb, &child_inode, records[i].inode_id);
        }
    }

    // delete myself
    free_block(sb, block_id);
    erase_block(fd, block_id);

    free_inode(sb, inode_id);
    erase_inode(fd, inode_id);
}

void delete_object(int output_fd, const char* fs_path, const char* path) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    cond_server_panic(fd == -1, "open error");

    char path_to_traverse[MAX_PATH_LENGTH];
    char obj_name[MAX_PATH_LENGTH];
    split_path(path, path_to_traverse, obj_name);

    cond_server_panic(
        strcmp(obj_name, ".") == 0 || strcmp(obj_name, "..") == 0,
        "can't delete \".\" or \"..\""
        );

    // get superblock
    struct superblock sb;
    reset_superblock(&sb);
    read_superblock(fd, &sb);

    // get inode of prev dir
    struct inode prev_inode;
    reset_inode(&prev_inode);
    traverse_path(fd, path_to_traverse, &prev_inode);

    // get inode of obj
    uint16_t inode_id;
    cond_server_panic(
        !check_if_obj_exists(
            fd,
            &prev_inode,
            prev_inode.block_ids[0],
            obj_name,
            &inode_id),
        "directory or file with such name doesn't exist"
    );
    struct inode obj_inode;
    reset_inode(&obj_inode);
    read_inode(fd, inode_id, &obj_inode);

    // delete obj
    if (obj_inode.is_file) {
        delete_file(fd, &sb, &obj_inode, inode_id);
    } else {
        delete_directory(fd, &sb, &obj_inode, inode_id);
    }

    // update parent dir
    uint16_t block_id = prev_inode.block_ids[0];

    struct dir_record records[prev_inode.size];
    read_dir_records(fd, block_id, records, prev_inode.size);

    struct dir_record new_records[prev_inode.size - 1];
    uint16_t j = 0;
    for (uint16_t i = 0; i < prev_inode.size; ++i) {
        if (strcmp(obj_name, records[i].name) != 0) {
            memcpy((char*)&new_records[j], (char*)&records[i], DIR_RECORD_SIZE);
            ++j;
        }
    }

    // update parent dir block
    erase_block(fd, block_id);
    write_dir_records(fd, block_id, new_records, prev_inode.size - 1);

    // update parent dir inode
    prev_inode.size -= 1;
    uint16_t prev_inode_id = new_records[0].inode_id;

    erase_inode(fd, prev_inode_id);
    write_inode(fd, prev_inode_id, &prev_inode);

    // update superblock
    write_superblock(fd, &sb);

    close(fd);
}
