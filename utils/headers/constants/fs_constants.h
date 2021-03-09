#ifndef MINI_FS_UTILS_FS_CONSTANTS_H_
#define MINI_FS_UTILS_FS_CONSTANTS_H_

#include <constants/struct_sizes.h>

#define FS_SIZE                  \
    (SUPERBLOCK_SIZE +           \
    DESCRIPTOR_TABLE_SIZE +      \
    INODE_SIZE * NUM_INODES +    \
    BLOCK_SIZE * NUM_BLOCKS)     \
// in bytes

#endif //MINI_FS_UTILS_FS_CONSTANTS_H_
