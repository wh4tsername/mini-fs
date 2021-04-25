#ifndef MINI_FS_FS_DIR_RECORD_H_
#define MINI_FS_FS_DIR_RECORD_H_

#include <stdint.h>

#define DIR_NAME_LENGTH 14

struct __attribute__((__packed__)) dir_record {
  uint16_t inode_id;
  char name[DIR_NAME_LENGTH];
};

void reset_dir_record(struct dir_record* dir_record_pointer);

#endif  // MINI_FS_FS_DIR_RECORD_H_
