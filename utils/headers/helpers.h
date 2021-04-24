#ifndef MINI_FS_UTILS_HELPERS_H_
#define MINI_FS_UTILS_HELPERS_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "../../fs/structures/inode.h"
#include "../../fs/structures/superblock.h"

uint16_t create_dir_block_and_inode(int fd,
                                    struct superblock* sb,
                                    bool is_root,
                                    uint16_t prev_inode_id);

void traverse_path(int fd, const char* path, struct inode* res);

char* parse_path(const char* path, char* next_token);

void split_path(const char* path,
                char* path_to_traverse,
                char* dir_name);

#endif //MINI_FS_UTILS_HELPERS_H_
