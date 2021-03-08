#ifndef MINI_FS__UTILS_H_
#define MINI_FS__UTILS_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "../../fs/dir_record.h"
#include "../../fs/inode.h"
#include "../../fs/superblock.h"
#include "../../fs/descriptor_table.h"

int write_retry(int fd, const char* buffer, int buffer_size);

int read_retry(int fd, char* buffer, int buffer_size);

void write_to_superblock(int fd, const struct superblock* sb);

void read_from_superblock(int fd, struct superblock* sb);

void write_to_descriptor_table(int fd, struct descriptor_table* dt);

void read_from_descriptor_table(int fd, struct descriptor_table* dt);

    void write_to_inode(int fd,
                    uint16_t inode_id,
                    const struct inode* inode);

void read_from_inode(int fd,
                     uint16_t inode_id,
                     struct inode* inode);

void write_to_block(int fd,
                    uint16_t block_id,
                    uint16_t inblock_offset,
                    const char* buffer,
                    int buffer_size);

void read_from_block(int fd,
                     uint16_t block_id,
                     uint16_t inblock_offset,
                     char* buffer,
                     int buffer_size);

void write_dir_records(int fd,
                       uint16_t block_id,
                       struct dir_record* records,
                       uint16_t num_records);

void read_dir_records(int fd,
                      uint16_t block_id,
                      struct dir_record* records,
                      uint16_t num_records);

void make_zeros(int fd, uint32_t offset, uint32_t size);

void erase_block(int fd, uint16_t block_id);

void erase_inode(int fd, uint16_t inode_id);

#endif //MINI_FS__UTILS_H_
