#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <disk_utils.h>

void seek_pos(const char* fs_path, uint16_t file_descr, uint32_t pos) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    read_descriptor_table(fd, &dt);

    // get offset
    struct inode inode;
    reset_inode(&inode);
    read_inode(fd, dt.inode_id[file_descr], &inode);

    conditional_handle_error(pos >= inode.size,
                             "position is out of bound");

    // change offset in descriptor table
    change_pos(&dt, file_descr, pos);

    // write descriptor table
    write_descriptor_table(fd, &dt);

    close(fd);
}
