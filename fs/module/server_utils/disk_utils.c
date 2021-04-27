#include "disk_utils.h"

#include <linux/string.h>

#include "../constants/constants.h"
#include "../constants/struct_sizes.h"
#include "fs_io_utils.h"
#include "module_defines.h"

int write_superblock(char* memory, const struct superblock* sb) {
  write_retry(memory, (const char*)sb, SUPERBLOCK_SIZE, 0);

  return 0;
}

int read_superblock(const char* memory, struct superblock* sb) {
  read_retry(memory, (char*)sb, SUPERBLOCK_SIZE, 0);

  return 0;
}

int write_descriptor_table(char* memory, const struct descriptor_table* dt) {
  write_retry(memory, (const char*)dt, DESCRIPTOR_TABLE_SIZE, SUPERBLOCK_SIZE);

  return 0;
}

int read_descriptor_table(const char* memory, struct descriptor_table* dt) {
  read_retry(memory, (char*)dt, DESCRIPTOR_TABLE_SIZE, SUPERBLOCK_SIZE);

  return 0;
}

int write_inode(char* memory, __u16 inode_id, const struct fs_inode* inode) {
  __u32 offset =
      SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE + INODE_SIZE * (inode_id - 1);

  cond_server_panic(inode_id > NUM_INODES || inode_id == 0,
                    "incorrect inode_id");
  write_retry(memory, (const char*)inode, INODE_SIZE, offset);

  return 0;
}

int read_inode(const char* memory, __u16 inode_id, struct fs_inode* inode) {
  __u32 offset =
      SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE + INODE_SIZE * (inode_id - 1);

  cond_server_panic(inode_id > NUM_INODES || inode_id == 0,
                    "incorrect inode_id");

  read_retry(memory, (char*)inode, INODE_SIZE, offset);

  return 0;
}

int write_block(char* memory, __u16 block_id, __u16 inblock_offset,
                const char* buffer, int buffer_size) {
  __u32 offset = SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE +
                 INODE_SIZE * NUM_INODES + BLOCK_SIZE * (block_id - 1) +
                 inblock_offset;

  cond_server_panic(block_id > NUM_BLOCKS || block_id == 0,
                    "incorrect block_id");
  cond_server_panic(inblock_offset + buffer_size > BLOCK_SIZE,
                    "write to block out of bounds");

  write_retry(memory, buffer, buffer_size, offset);

  return 0;
}

int read_block(const char* memory, __u16 block_id, __u16 inblock_offset,
               char* buffer, int buffer_size) {
  __u32 offset = SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE +
                 INODE_SIZE * NUM_INODES + BLOCK_SIZE * (block_id - 1) +
                 inblock_offset;

  cond_server_panic(block_id > NUM_BLOCKS || block_id == 0,
                    "incorrect block_id");
  cond_server_panic(inblock_offset + buffer_size > BLOCK_SIZE,
                    "read from block out of bounds");

  read_retry(memory, buffer, buffer_size, offset);

  return 0;
}

int write_dir_records(char* memory, __u16 block_id, struct dir_record* records,
                      __u16 num_records) {
  for (__u16 i = 0; i < num_records; ++i) {
    check_ret_code(write_block(memory, block_id, i * DIR_RECORD_SIZE,
                               (const char*)&records[i], DIR_RECORD_SIZE));
  }

  return 0;
}

int read_dir_records(const char* memory, __u16 block_id,
                     struct dir_record* records, __u16 num_records) {
  for (__u16 i = 0; i < num_records; ++i) {
    check_ret_code(read_block(memory, block_id, i * DIR_RECORD_SIZE,
                              (char*)&records[i], DIR_RECORD_SIZE));
  }

  return 0;
}

int erase_block(char* memory, __u16 block_id) {
  __u32 offset = SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE +
                 INODE_SIZE * NUM_INODES + BLOCK_SIZE * (block_id - 1);

  cond_server_panic(block_id > NUM_BLOCKS || block_id == 0,
                    "incorrect block_id");

  memset(memory + offset, 0, BLOCK_SIZE);

  return 0;
}

int erase_inode(char* memory, __u16 inode_id) {
  struct fs_inode inode;
  reset_inode(&inode);

  check_ret_code(write_inode(memory, inode_id, &inode));

  return 0;
}
