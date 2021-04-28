#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int seek_pos(char* results, char* memory, __u16 file_descr, __u32 pos) {
  // get descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));

  check_ret_code(read_descriptor_table(memory, &dt));

  // get offset
  struct fs_inode inode;
  check_ret_code(reset_inode(&inode));
  check_ret_code(read_inode(memory, dt.inode_id[file_descr], &inode));

  cond_server_panic(pos >= inode.size, "position is out of bound");

  // change offset in descriptor table
  check_ret_code(change_pos(&dt, file_descr, pos));

  // write descriptor table
  check_ret_code(write_descriptor_table(memory, &dt));

  return 0;
}
