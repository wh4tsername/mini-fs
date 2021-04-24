#include <defines.h>
#include <disk_utils.h>
#include <helpers.h>
#include <stdlib.h>

#include "handlers.h"

void list_dir(int output_fd, const char* fs_path, const char* path) {
  int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  conditional_parse_errno(fd == -1);

  struct inode inode;
  traverse_path(fd, path, &inode);

  uint16_t block_id = inode.block_ids[0];

  struct dir_record records[16];
  read_dir_records(fd, block_id, records, inode.size);

  dprintf(output_fd, "%u", (unsigned char)inode.size);
  for (uint16_t i = 0; i < inode.size; ++i) {
    struct inode obj_inode;
    reset_inode(&obj_inode);
    read_inode(fd, records[i].inode_id, &obj_inode);

    dprintf(output_fd, (obj_inode.is_file ? "%s - file\n" : "%s\n"),
            records[i].name);
  }

  close(fd);
}
