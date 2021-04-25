#include "../server_utils/helpers.h"

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

void close_file(int output_fd, const char* fs_path, uint16_t file_descr) {
  int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  // get descriptor table
  struct descriptor_table dt;
  reset_descriptor_table(&dt);

  read_descriptor_table(fd, &dt);

  // free file descriptor
  free_descriptor(&dt, file_descr);

  // write descriptor table
  write_descriptor_table(fd, &dt);

  close(fd);
}
