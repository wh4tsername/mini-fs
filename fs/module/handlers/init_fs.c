#include <linux/string.h>

#include "../constants/constants.h"
#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int make_partition(char* memory) {
  // create superblock
  struct superblock sb;
  check_ret_code(reset_superblock(&sb));

  __u16 inode_id = 0;
  check_ret_code(create_dir_block_and_inode(memory, &sb, true, 0, &inode_id));

  // write superblock
  check_ret_code(write_superblock(memory, &sb));

  // create and write descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));

  check_ret_code(write_descriptor_table(memory, &dt));

  return 0;
}

int init_fs(char* results, char* memory) {
  memset(memory, 0, FS_SIZE);

  check_ret_code(make_partition(memory));

  return 0;
}
