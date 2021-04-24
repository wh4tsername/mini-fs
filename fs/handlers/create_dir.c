#include "handlers.h"

#include <defines.h>
#include <disk_utils.h>
#include <helpers.h>
#include <constants/fs_constants.h>

bool check_for_duplicate_dir(int fd,
                             struct inode* inode,
                             uint16_t block_id,
                             const char* name) {
    struct dir_record records[NUM_RECORDS_IN_DIR];
    read_dir_records(fd, block_id, records, inode->size);

    for (uint16_t i = 0; i < inode->size; ++i) {
        if (strcmp(name, records[i].name) == 0) {
            return true;
        }
    }

    return false;
}

void create_dir(const char* fs_path, const char* path) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    char path_to_traverse[MAX_PATH_LENGTH];
    char dir_name[MAX_PATH_LENGTH];
    split_path(path, path_to_traverse, dir_name);

//    printf("%s\n", path_to_traverse);
//    printf("%s\n", dir_name);

    // get superblock
    struct superblock sb;
    reset_superblock(&sb);
    read_superblock(fd, &sb);

    // get inode of prev dir
    struct inode inode;
    reset_inode(&inode);
    traverse_path(fd, path_to_traverse, &inode);

    // get block of prev dir
    struct dir_record record;
    reset_dir_record(&record);
    read_block(fd,
               inode.block_ids[0],
               0,
               (char *) &record,
               DIR_RECORD_SIZE);
    uint16_t inode_id = record.inode_id;

    conditional_handle_error(
        check_for_duplicate_dir(fd, &inode, inode.block_ids[0], dir_name),
        "directory with such name already exists"
        );

    uint16_t next_inode_id = create_dir_block_and_inode(fd,
                                                        &sb,
                                                        false,
                                                        inode_id);

    // create dir_record for prev dir
    reset_dir_record(&record);
    record.inode_id = next_inode_id;
    strcpy(record.name, dir_name);

    // inode of prev dir info update
    conditional_handle_error(inode.size >= NUM_RECORDS_IN_DIR, "directory overflow");
    inode.size += 1;
    write_inode(fd,
                inode_id,
                &inode);

    // make new dir_record
    write_block(fd,
                inode.block_ids[0],
                (inode.size - 1) * DIR_RECORD_SIZE,
                (const char *) &record,
                DIR_RECORD_SIZE);

    // write superblock
    write_superblock(fd, &sb);

    close(fd);
}
