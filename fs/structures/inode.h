#ifndef MINI_FS_FS_INODE_H_
#define MINI_FS_FS_INODE_H_

#include <stdint.h>
#include <stdbool.h>

#define NUM_BLOCK_IDS_IN_INODE 8

struct __attribute__((__packed__)) inode {
  uint16_t block_ids[NUM_BLOCK_IDS_IN_INODE];
  uint16_t inode_id;
  uint32_t size;
  bool is_file;
};

void reset_inode(struct inode* inode_pointer);

#endif //MINI_FS_FS_INODE_H_
