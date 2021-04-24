#ifndef MINI_FS_UTILS_STRUCT_SIZES_H_
#define MINI_FS_UTILS_STRUCT_SIZES_H_

#include "../../../fs/structures/superblock.h"
#include "../../../fs/structures/inode.h"
#include "../../../fs/structures/dir_record.h"
#include "../../../fs/structures/descriptor_table.h"

#define SUPERBLOCK_SIZE (sizeof(struct superblock))
#define INODE_SIZE (sizeof(struct inode))
#define DIR_RECORD_SIZE (sizeof(struct dir_record))
#define DESCRIPTOR_TABLE_SIZE (sizeof(struct descriptor_table))

#endif //MINI_FS_UTILS_STRUCT_SIZES_H_
