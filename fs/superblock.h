#ifndef MINI_FS_FS_SUPERBLOCK_H_
#define MINI_FS_FS_SUPERBLOCK_H_

#include <stdbool.h>

#include <constants/constants.h>
#include "inode.h"

struct superblock {
  bool occupied_blocks_mask[NUM_BLOCKS];
  bool occupied_inodes_mask[NUM_INODES];
};

void reset_superblock(struct superblock* sb_pointer);

uint16_t occupy_block(struct superblock* sb_pointer);

void free_block(struct superblock* sb_pointer, uint16_t block_id);

uint16_t occupy_inode(struct superblock* sb_pointer);

void free_inode(struct superblock* sb_pointer, uint16_t inode_id);

#endif //MINI_FS_FS_SUPERBLOCK_H_
