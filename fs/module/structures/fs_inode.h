#ifndef MINI_FS_FS_INODE_H_
#define MINI_FS_FS_INODE_H_

#include <linux/types.h>
#include "../server_utils/fs_types.h"

#define NUM_BLOCK_IDS_IN_INODE 8

struct __attribute__((__packed__)) fs_inode {
  __u16 block_ids[NUM_BLOCK_IDS_IN_INODE];
  __u16 inode_id;
  __u32 size;
  bool is_file;
};

int reset_inode(struct fs_inode* inode_pointer);

#endif  // MINI_FS_FS_INODE_H_
