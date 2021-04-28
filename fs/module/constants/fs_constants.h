#ifndef MINI_FS_UTILS_FS_CONSTANTS_H_
#define MINI_FS_UTILS_FS_CONSTANTS_H_

#include "struct_sizes.h"

#define FS_SIZE                                                        \
  (SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE + INODE_SIZE * NUM_INODES + \
   FS_BLOCK_SIZE * NUM_BLOCKS)  // in bytes

//#define FS_FILE "/root/fs.bin"

#endif  // MINI_FS_UTILS_FS_CONSTANTS_H_
