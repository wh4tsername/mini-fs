#ifndef MINI_FS_UTILS_CONSTANTS_H_
#define MINI_FS_UTILS_CONSTANTS_H_

#include <stdint.h>

#define BLOCK_SIZE 48

// BLOCK_SIZE / DIR_RECORD_SIZE !!!
#define NUM_RECORDS_IN_DIR 3

// < uint16_t
#define NUM_INODES 128
// < uint16_t
#define NUM_BLOCKS 128

#endif //MINI_FS_UTILS_CONSTANTS_H_
