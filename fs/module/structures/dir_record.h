#ifndef MINI_FS_FS_DIR_RECORD_H_
#define MINI_FS_FS_DIR_RECORD_H_

#include <linux/types.h>

#define DIR_NAME_LENGTH 14

struct __attribute__((__packed__)) dir_record {
  __u16 inode_id;
  char name[DIR_NAME_LENGTH];
};

int reset_dir_record(struct dir_record* dir_record_pointer);

#endif  // MINI_FS_FS_DIR_RECORD_H_
