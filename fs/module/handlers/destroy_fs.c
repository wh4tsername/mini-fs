#include "../constants/fs_constants.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

void destroy_fs(int output_fd, const char* fs_path) {
  int fd = open(fs_path, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  ftruncate(fd, FS_SIZE);

  close(fd);
}
