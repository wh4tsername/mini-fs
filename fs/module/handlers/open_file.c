#include <constants/fs_constants.h>
#include "../server_utils/helpers.h"

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

bool check_for_file(int fd, struct inode* inode, uint16_t block_id,
                    const char* name, uint16_t* inode_id) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  read_dir_records(fd, block_id, records, inode->size);

  for (uint16_t i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      *inode_id = records[i].inode_id;
      return true;
    }
  }

  return false;
}

void open_file(int output_fd, const char* fs_path, const char* path) {
  int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  char path_to_traverse[MAX_PATH_LENGTH];
  char file_name[MAX_PATH_LENGTH];
  split_path(path, path_to_traverse, file_name);

  // get inode of prev dir
  struct inode prev_inode;
  reset_inode(&prev_inode);
  traverse_path(fd, path_to_traverse, &prev_inode);

  // get block of prev dir
  struct dir_record record;
  reset_dir_record(&record);
  read_block(fd, prev_inode.block_ids[0], 0, (char*)&record, DIR_RECORD_SIZE);
  uint16_t prev_inode_id = record.inode_id;

  // get inode_id of file
  uint16_t inode_id = 0;
  cond_server_panic(!check_for_file(fd, &prev_inode, prev_inode.block_ids[0],
                                    file_name, &inode_id),
                    "file with such name doesn't exist");
  struct inode file_inode;
  reset_inode(&file_inode);
  read_inode(fd, inode_id, &file_inode);

  cond_server_panic(!file_inode.is_file, "can open only files");

  // get descriptor table
  struct descriptor_table dt;
  reset_descriptor_table(&dt);

  read_descriptor_table(fd, &dt);

  // occupy file descriptor
  uint16_t descr = occupy_descriptor(&dt, inode_id);
  dprintf(output_fd, "%u", (unsigned char)1);
  dprintf(output_fd, "fd: %hu\n", descr);

  // write descriptor table
  write_descriptor_table(fd, &dt);

  close(fd);
}