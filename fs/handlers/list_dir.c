#include "handlers.h"
#include <utils.h>
#include <helpers.h>
#include <defines.h>
#include <constants/fs_constants.h>

#include <stdlib.h>

void list_dir(const char* fs_path, const char* path) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    struct inode inode;
    traverse_path(fd, path, &inode);

    uint16_t block_id = inode.block_ids[0];

    struct dir_record records[16];
    read_dir_records(fd, block_id, records, inode.size);

    for(uint16_t i = 0; i < inode.size; ++i) {
        printf("%s", records[i].name);

        struct inode obj_inode;
        reset_inode(&obj_inode);
        read_inode(fd, records[i].inode_id, &obj_inode);

        if (obj_inode.is_file) {
            printf(" - file");
        }
        printf("\n");
    }

    close(fd);
}
