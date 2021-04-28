#include "superblock.h"

#include "../server_utils/module_defines.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int reset_superblock(struct superblock* sb_pointer) {
  for (__u16 i = 0; i < NUM_BLOCKS; ++i) {
    sb_pointer->occupied_blocks_mask[i] = false;
  }
  for (__u16 i = 0; i < NUM_INODES; ++i) {
    sb_pointer->occupied_inodes_mask[i] = false;
  }

  return 0;
}

int occupy_block(struct superblock* sb_pointer, __u16* block_id) {
  for (__u16 i = 0; i < NUM_BLOCKS; ++i) {
    if (sb_pointer->occupied_blocks_mask[i] == false) {
      sb_pointer->occupied_blocks_mask[i] = true;
      *block_id = i + 1;

      return 0;
    }
  }

  server_panic("no empty blocks to occupy");
}

int free_block(struct superblock* sb_pointer, __u16 block_id) {
  cond_server_panic(
      sb_pointer->occupied_blocks_mask[block_id - 1] == false,
      "trying to double free block");

  sb_pointer->occupied_blocks_mask[block_id - 1] = false;

  return 0;
}

int occupy_inode(struct superblock* sb_pointer, __u16* inode_id) {
  for (__u16 i = 0; i < NUM_INODES; ++i) {
    if (sb_pointer->occupied_inodes_mask[i] == false) {
      sb_pointer->occupied_inodes_mask[i] = true;
      *inode_id = i + 1;

      return 0;
    }
  }

  server_panic("no empty inodes to occupy");
}

int free_inode(struct superblock* sb_pointer, __u16 inode_id) {
  cond_server_panic(
      sb_pointer->occupied_inodes_mask[inode_id - 1] == false,
      "trying to double free inode");

  sb_pointer->occupied_inodes_mask[inode_id - 1] = false;

  return 0;
}
