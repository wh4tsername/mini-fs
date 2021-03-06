#include "handlers.h"

#include <defines.h>
#include <utils.h>
#include <helpers.h>
#include <constants/fs_constants.h>

bool check_for_duplicate_dir(int fd,
                             struct inode* inode,
                             uint16_t block_id,
                             const char* name) {
    struct dir_record records[16];
    read_dir_records(fd, block_id, records, inode->size);

    for (uint16_t i = 0; i < inode->size; ++i) {
        if (strcmp(name, records[i].name) == 0) {
            return true;
        }
    }

    return false;
}

void create_dir(const char* path) {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    char path_to_traverse[256];
    char dir_name[256];
    split_path(path, path_to_traverse, dir_name);

//    printf("%s\n", path_to_traverse);
//    printf("%s\n", dir_name);

    // get superblock
    struct superblock sb;
    init_superblock(&sb);
    read_from_superblock(fd, &sb);

    // get inode of prev dir
    struct inode inode;
    init_inode(&inode);
    traverse_path(fd, path_to_traverse, &inode);

    // get block of prev dir
    struct dir_record record;
    init_dir_record(&record);
    read_from_block(fd,
                    inode.block_ids[0],
                    0,
                    (char*)&record,
                    DIR_RECORD_SIZE);
    uint16_t inode_id = record.inode_id;

    conditional_handle_error(
        check_for_duplicate_dir(fd, &inode, inode.block_ids[0], dir_name),
        "directory with such name already exists"
        );

    uint16_t next_inode_id = create_dir_block_and_inode(fd, &sb, false, inode_id);

    // create dir_record for prev dir
    init_dir_record(&record);
    record.inode_id = next_inode_id;
    strcpy(record.name, dir_name);

    // inode of prev dir info update
    conditional_handle_error(inode.size >= 16, "directory overflow");
    inode.size += 1;
    write_to_inode(fd,
                   inode_id,
                   &inode);

    // make new dir_record
    write_to_block(fd,
                   inode.block_ids[0],
                   (inode.size - 1) * DIR_RECORD_SIZE,
                   (const char *)&record,
                   DIR_RECORD_SIZE);

    // write superblock
    write_to_superblock(fd, &sb);

    close(fd);
}
