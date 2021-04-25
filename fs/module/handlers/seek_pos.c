#include <constants/fs_constants.h>
#include <helpers.h>

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

void seek_pos(int output_fd, const char* fs_path, uint16_t file_descr,
              uint32_t pos) {
  int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  // get descriptor table
  struct descriptor_table dt;
  reset_descriptor_table(&dt);

  read_descriptor_table(fd, &dt);

  // get offset
  struct inode inode;
  reset_inode(&inode);
  read_inode(fd, dt.inode_id[file_descr], &inode);

  cond_server_panic(pos >= inode.size, "position is out of bound");

  // change offset in descriptor table
  change_pos(&dt, file_descr, pos);

  // write descriptor table
  write_descriptor_table(fd, &dt);

  close(fd);
}
