#include "inode.h"

void reset_inode(struct inode* inode_pointer) {
  for (int i = 0; i < NUM_BLOCK_IDS_IN_INODE; ++i) {
    inode_pointer->block_ids[i] = 0;
  }
  inode_pointer->inode_id = 0;
  inode_pointer->size = 0;
  inode_pointer->is_file = false;
}
