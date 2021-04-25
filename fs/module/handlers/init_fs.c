#include <constants/constants.h>
#include <constants/fs_constants.h>
#include <helpers.h>

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "../structures/dir_record.h"
#include "../structures/superblock.h"
#include "handlers.h"

void make_partition(int fd) {
  // create superblock
  struct superblock sb;
  reset_superblock(&sb);

  create_dir_block_and_inode(fd, &sb, true, 0);

  // write superblock
  write_superblock(fd, &sb);

  // create and write descriptor table
  struct descriptor_table dt;
  reset_descriptor_table(&dt);

  write_descriptor_table(fd, &dt);
}

void init_fs(int output_fd, const char* fs_path) {
  int fd = open(fs_path, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  ftruncate(fd, FS_SIZE);

  make_partition(fd);

  close(fd);
}
