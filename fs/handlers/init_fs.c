#include "handlers.h"
#include "../../utils/defines.h"
#include "../../utils/utils.h"
#include "../../utils/constants/constants.h"
#include "../../utils/constants/fs_constants.h"
#include "../superblock.h"
#include "../dir_record.h"

void make_partition(int fd) {
    // create superblock
    struct superblock sb;
    init_superblock(&sb);

    uint16_t block_id = occupy_block(&sb);
    uint16_t inode_id = occupy_inode(&sb);

    // create dir_records
    struct dir_record records[2];
    records[0].inode_id = inode_id;
    records[1].inode_id = inode_id;
    strcpy(records[0].name,".");
    strcpy(records[1].name,"..");

    write_dir_records(fd, block_id, records, 2);

    // write superblock
    lseek(fd, 0, SEEK_SET);
    conditional_handle_error(write_retry(fd, (const char*)&sb, SUPERBLOCK_SIZE) == -1,
                             "error while writing to file");
}

void init_fs() {
    int fd = open(FS_FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    make_partition(fd);

    close(fd);
}
