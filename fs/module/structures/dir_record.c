#include "dir_record.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int reset_dir_record(struct dir_record* dir_record_pointer) {
  dir_record_pointer->inode_id = 0;

  for (int i = 0; i < DIR_NAME_LENGTH; ++i) {
    dir_record_pointer->name[i] = '\0';
  }

  return 0;
}
