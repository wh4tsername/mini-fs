#ifndef MINI_FS_FS_DESCRIPTOR_TABLE_H_
#define MINI_FS_FS_DESCRIPTOR_TABLE_H_

#include <stdint.h>
#include <stdbool.h>

#define NUM_DESCRIPTORS 16

struct __attribute__((__packed__)) descriptor_table {
  bool fd_mask[NUM_DESCRIPTORS];
  uint16_t inode_id[NUM_DESCRIPTORS];
  uint32_t pos[NUM_DESCRIPTORS];
};

void reset_descriptor_table(struct descriptor_table* dt);

uint16_t occupy_descriptor(
        struct descriptor_table* dt,
        uint16_t inode_id);

void free_descriptor(struct  descriptor_table* dt, uint16_t descr);

void change_pos(struct descriptor_table* dt,
                uint16_t descr,
                uint32_t pos);

#endif //MINI_FS_FS_DESCRIPTOR_TABLE_H_
