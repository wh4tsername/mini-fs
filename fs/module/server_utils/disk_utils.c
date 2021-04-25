#include "disk_utils.h"

#include <constants/constants.h>
#include <constants/struct_sizes.h>
#include <defines.h>
#include <io_utils.h>
#include <string.h>

void write_superblock(int fd, const struct superblock* sb) {
  lseek(fd, 0, SEEK_SET);
  conditional_parse_errno(write_retry(fd, (const char*)sb, SUPERBLOCK_SIZE) ==
                          -1);
}

void read_superblock(int fd, struct superblock* sb) {
  lseek(fd, 0, SEEK_SET);
  conditional_parse_errno(read_retry(fd, (char*)sb, SUPERBLOCK_SIZE) == -1);
}

void write_descriptor_table(int fd, struct descriptor_table* dt) {
  lseek(fd, SUPERBLOCK_SIZE, SEEK_SET);
  conditional_parse_errno(
      write_retry(fd, (const char*)dt, DESCRIPTOR_TABLE_SIZE) == -1);
}

void read_descriptor_table(int fd, struct descriptor_table* dt) {
  lseek(fd, SUPERBLOCK_SIZE, SEEK_SET);
  conditional_parse_errno(read_retry(fd, (char*)dt, DESCRIPTOR_TABLE_SIZE) ==
                          -1);
}

void write_inode(int fd, uint16_t inode_id, const struct inode* inode) {
  uint32_t offset =
      SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE + INODE_SIZE * (inode_id - 1);

  conditional_handle_error(inode_id > NUM_INODES || inode_id == 0,
                           "incorrect inode_id");

  lseek(fd, offset, SEEK_SET);
  conditional_parse_errno(write_retry(fd, (const char*)inode, INODE_SIZE) ==
                          -1);
}

void read_inode(int fd, uint16_t inode_id, struct inode* inode) {
  uint32_t offset =
      SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE + INODE_SIZE * (inode_id - 1);

  conditional_handle_error(inode_id > NUM_INODES || inode_id == 0,
                           "incorrect inode_id");

  lseek(fd, offset, SEEK_SET);
  conditional_parse_errno(read_retry(fd, (char*)inode, INODE_SIZE) == -1);
}

void write_block(int fd, uint16_t block_id, uint16_t inblock_offset,
                 const char* buffer, int buffer_size) {
  uint32_t offset = SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE +
                    INODE_SIZE * NUM_INODES + BLOCK_SIZE * (block_id - 1) +
                    inblock_offset;

  conditional_handle_error(block_id > NUM_BLOCKS || block_id == 0,
                           "incorrect block_id");
  conditional_handle_error(inblock_offset + buffer_size > BLOCK_SIZE,
                           "write to block out of bounds");

  lseek(fd, offset, SEEK_SET);
  conditional_parse_errno(write_retry(fd, buffer, buffer_size) == -1);
}

void read_block(int fd, uint16_t block_id, uint16_t inblock_offset,
                char* buffer, int buffer_size) {
  uint32_t offset = SUPERBLOCK_SIZE + DESCRIPTOR_TABLE_SIZE +
                    INODE_SIZE * NUM_INODES + BLOCK_SIZE * (block_id - 1) +
                    inblock_offset;

  conditional_handle_error(block_id > NUM_BLOCKS || block_id == 0,
                           "incorrect block_id");
  conditional_handle_error(inblock_offset + buffer_size > BLOCK_SIZE,
                           "read from block out of bounds");

  lseek(fd, offset, SEEK_SET);
  conditional_parse_errno(read_retry(fd, buffer, buffer_size) == -1);
}

void write_dir_records(int fd, uint16_t block_id, struct dir_record* records,
                       uint16_t num_records) {
  for (uint16_t i = 0; i < num_records; ++i) {
    write_block(fd, block_id, i * DIR_RECORD_SIZE, (const char*)&records[i],
                DIR_RECORD_SIZE);
  }
}

void read_dir_records(int fd, uint16_t block_id, struct dir_record* records,
                      uint16_t num_records) {
  for (uint16_t i = 0; i < num_records; ++i) {
    read_block(fd, block_id, i * DIR_RECORD_SIZE, (char*)&records[i],
               DIR_RECORD_SIZE);
  }
}

void erase_block(int fd, uint16_t block_id) {
  uint32_t size = BLOCK_SIZE;

  char* buffer = malloc(size);
  memset(buffer, '\0', size);

  write_block(fd, block_id, 0, buffer, size);

  free(buffer);
}

void erase_inode(int fd, uint16_t inode_id) {
  struct inode inode;
  reset_inode(&inode);

  write_inode(fd, inode_id, &inode);
}
