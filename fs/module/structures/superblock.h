#ifndef MINI_FS_FS_SUPERBLOCK_H_
#define MINI_FS_FS_SUPERBLOCK_H_

#include "../constants/constants.h"
#include <linux/types.h>

#include "fs_inode.h"

struct __attribute__((__packed__)) superblock {
  bool occupied_blocks_mask[NUM_BLOCKS];
  bool occupied_inodes_mask[NUM_INODES];
};

int reset_superblock(struct superblock* sb_pointer);

int occupy_block(struct superblock* sb_pointer, __u16* block_id);

int free_block(struct superblock* sb_pointer, __u16 block_id);

int occupy_inode(struct superblock* sb_pointer, __u16* block_id);

int free_inode(struct superblock* sb_pointer, __u16 inode_id);

#endif  // MINI_FS_FS_SUPERBLOCK_H_
