#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <utils.h>

void seek_pos(uint16_t file_descr, bool pos_is_start) {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    read_from_descriptor_table(fd, &dt);

    // get offset
    struct inode inode;
    reset_inode(&inode);
    read_from_inode(fd, dt.inode_id[file_descr], &inode);

    uint32_t pos = pos_is_start ? 0 : inode.size;

    // change offset in descriptor table
    change_pos(&dt, file_descr, pos);

    // write descriptor table
    write_to_descriptor_table(fd, &dt);

    close(fd);
}
