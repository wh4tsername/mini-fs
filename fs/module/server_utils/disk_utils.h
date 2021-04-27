#ifndef MINI_FS__UTILS_H_
#define MINI_FS__UTILS_H_

#include <linux/types.h>

#include "../structures/descriptor_table.h"
#include "../structures/dir_record.h"
#include "../structures/fs_inode.h"
#include "../structures/superblock.h"

int write_superblock(char* memory, const struct superblock* sb);

int read_superblock(const char* memory, struct superblock* sb);

int write_descriptor_table(char* memory, const struct descriptor_table* dt);

int read_descriptor_table(const char* memory, struct descriptor_table* dt);

int write_inode(char* memory, __u16 inode_id, const struct fs_inode* inode);

int read_inode(const char* memory, __u16 inode_id, struct fs_inode* inode);

int write_block(char* memory, __u16 block_id, __u16 inblock_offset,
                 const char* buffer, int buffer_size);

int read_block(const char* memory, __u16 block_id, __u16 inblock_offset,
                char* buffer, int buffer_size);

int write_dir_records(char* memory, __u16 block_id, struct dir_record* records,
                       __u16 num_records);

int read_dir_records(const char* memory, __u16 block_id, struct dir_record* records,
                      __u16 num_records);

int erase_block(char* memory, __u16 block_id);

int erase_inode(char* memory, __u16 inode_id);

#endif  // MINI_FS__UTILS_H_
