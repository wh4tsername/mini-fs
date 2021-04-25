#ifndef MINI_FS__UTILS_H_
#define MINI_FS__UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "../structures/descriptor_table.h"
#include "../structures/dir_record.h"
#include "../structures/fs_inode.h"
#include "../structures/superblock.h"

void write_superblock(int fd, const struct superblock* sb);

void read_superblock(int fd, struct superblock* sb);

void write_descriptor_table(int fd, struct descriptor_table* dt);

void read_descriptor_table(int fd, struct descriptor_table* dt);

void write_inode(int fd, uint16_t inode_id, const struct fs_inode* inode);

void read_inode(int fd, uint16_t inode_id, struct fs_inode* inode);

void write_block(int fd, uint16_t block_id, uint16_t inblock_offset,
                 const char* buffer, int buffer_size);

void read_block(int fd, uint16_t block_id, uint16_t inblock_offset,
                char* buffer, int buffer_size);

void write_dir_records(int fd, uint16_t block_id, struct dir_record* records,
                       uint16_t num_records);

void read_dir_records(int fd, uint16_t block_id, struct dir_record* records,
                      uint16_t num_records);

void erase_block(int fd, uint16_t block_id);

void erase_inode(int fd, uint16_t inode_id);

#endif  // MINI_FS__UTILS_H_
