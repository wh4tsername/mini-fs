#ifndef MINI_FS_UTILS_STRUCT_SIZES_H_
#define MINI_FS_UTILS_STRUCT_SIZES_H_

#include "../../../fs/superblock.h"
#include "../../../fs/inode.h"
#include "../../../fs/dir_record.h"

#define SUPERBLOCK_SIZE (sizeof(struct superblock))
#define INODE_SIZE (sizeof(struct inode))
#define DIR_RECORD_SIZE (sizeof(struct dir_record))

#endif //MINI_FS_UTILS_STRUCT_SIZES_H_
