#ifndef MINI_FS_UTILS_CONSTANTS_H_
#define MINI_FS_UTILS_CONSTANTS_H_

#include <stdint.h>

#define BLOCK_SIZE 512

// BLOCK_SIZE / DIR_RECORD_SIZE
#define NUM_RECORDS_IN_DIR 32

// < uint16_t
#define NUM_INODES 128
// < uint16_t
#define NUM_BLOCKS 256

#define MAX_PATH_LENGTH 256

#endif //MINI_FS_UTILS_CONSTANTS_H_
