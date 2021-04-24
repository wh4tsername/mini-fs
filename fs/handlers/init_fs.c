#include "handlers.h"
#include <defines.h>
#include <disk_utils.h>
#include <helpers.h>
#include <constants/constants.h>
#include <constants/fs_constants.h>
#include "../structures/superblock.h"
#include "../structures/dir_record.h"

void make_partition(int fd) {
    // create superblock
    struct superblock sb;
    reset_superblock(&sb);

    create_dir_block_and_inode(fd, &sb, true, 0);

    // write superblock
    write_superblock(fd, &sb);

    // create and write descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    write_descriptor_table(fd, &dt);
}

void init_fs(const char* fs_path) {
    int fd = open(fs_path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    make_partition(fd);

    close(fd);
}
