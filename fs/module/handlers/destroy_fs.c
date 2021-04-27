#include "../constants/fs_constants.h"
#include "handlers.h"

#include <linux/string.h>

int destroy_fs(char* results, char* memory) {
  memset(memory, 0, FS_SIZE);

  return 0;
}
