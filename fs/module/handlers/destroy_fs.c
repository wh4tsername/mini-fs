#include "../constants/fs_constants.h"
#include "handlers.h"

#include <linux/string.h>

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int destroy_fs(char* results, char* memory) {
  memset(memory, 0, FS_SIZE);

  return 0;
}
