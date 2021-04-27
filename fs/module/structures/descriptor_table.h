#ifndef MINI_FS_FS_DESCRIPTOR_TABLE_H_
#define MINI_FS_FS_DESCRIPTOR_TABLE_H_

#include <linux/types.h>

#include "../server_utils/fs_types.h"

#define NUM_DESCRIPTORS 16

struct __attribute__((__packed__)) descriptor_table {
  bool fd_mask[NUM_DESCRIPTORS];
  __u16 inode_id[NUM_DESCRIPTORS];
  __u32 pos[NUM_DESCRIPTORS];
};

int reset_descriptor_table(struct descriptor_table* dt);

int occupy_descriptor(struct descriptor_table* dt, __u16 inode_id, __u16* file_descr);

int free_descriptor(struct descriptor_table* dt, __u16 descr);

int change_pos(struct descriptor_table* dt, __u16 descr, __u32 pos);

#endif  // MINI_FS_FS_DESCRIPTOR_TABLE_H_
