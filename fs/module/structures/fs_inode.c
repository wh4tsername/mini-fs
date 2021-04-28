#include "fs_inode.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int reset_inode(struct fs_inode* inode_pointer) {
  for (int i = 0; i < NUM_BLOCK_IDS_IN_INODE; ++i) {
    inode_pointer->block_ids[i] = 0;
  }
  inode_pointer->inode_id = 0;
  inode_pointer->size = 0;
  inode_pointer->is_file = false;

  return 0;
}
