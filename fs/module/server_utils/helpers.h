#ifndef MINI_FS_UTILS_HELPERS_H_
#define MINI_FS_UTILS_HELPERS_H_

#include <linux/types.h>

#include "../structures/fs_inode.h"
#include "../structures/superblock.h"

int create_dir_block_and_inode(char* memory, struct superblock* sb, bool is_root,
                                 __u16 prev_inode_id, __u16* ret_inode_id);

int traverse_path(char* memory, const char* path, struct fs_inode* res);

int parse_path(const char* path, char* next_token, char** res);

int split_path(const char* path, char* path_to_traverse, char* dir_name);

#endif  // MINI_FS_UTILS_HELPERS_H_
