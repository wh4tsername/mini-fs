#include "handlers.h"
#include <defines.h>
#include <utils.h>
#include <helpers.h>
#include <constants/constants.h>
#include <constants/fs_constants.h>
#include "../superblock.h"
#include "../dir_record.h"

void make_partition(int fd) {
    // create superblock
    struct superblock sb;
    reset_superblock(&sb);

    create_dir_block_and_inode(fd, &sb, true, 0);

    // write superblock
    write_to_superblock(fd, &sb);
}

void init_fs() {
    int fd = open(FS_FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    make_partition(fd);

    close(fd);
}
