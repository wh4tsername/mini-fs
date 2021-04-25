#ifndef MINI_FS_UTILS_STRUCT_SIZES_H_
#define MINI_FS_UTILS_STRUCT_SIZES_H_

#include "../structures/descriptor_table.h"
#include "../structures/dir_record.h"
#include "../structures/fs_inode.h"
#include "../structures/superblock.h"

#define SUPERBLOCK_SIZE (sizeof(struct superblock))
#define INODE_SIZE (sizeof(struct fs_inode))
#define DIR_RECORD_SIZE (sizeof(struct dir_record))
#define DESCRIPTOR_TABLE_SIZE (sizeof(struct descriptor_table))

#endif  // MINI_FS_UTILS_STRUCT_SIZES_H_
