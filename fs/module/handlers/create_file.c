#include <constants/fs_constants.h>
#include "../server_utils/helpers.h"

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

bool check_for_duplicate_file(int fd, struct inode* inode, uint16_t block_id,
                              const char* name) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  read_dir_records(fd, block_id, records, inode->size);

  for (uint16_t i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      return true;
    }
  }

  return false;
}

void create_file(int output_fd, const char* fs_path, const char* path) {
  int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(fd == -1, "open error");

  char path_to_traverse[256];
  char file_name[256];
  split_path(path, path_to_traverse, file_name);

  // get superblock
  struct superblock sb;
  reset_superblock(&sb);
  read_superblock(fd, &sb);

  // get inode of prev dir
  struct inode prev_inode;
  reset_inode(&prev_inode);
  traverse_path(fd, path_to_traverse, &prev_inode);

  // get block of prev dir
  struct dir_record record;
  reset_dir_record(&record);
  read_block(fd, prev_inode.block_ids[0], 0, (char*)&record, DIR_RECORD_SIZE);
  uint16_t prev_inode_id = record.inode_id;

  cond_server_panic(check_for_duplicate_file(
                        fd, &prev_inode, prev_inode.block_ids[0], file_name),
                    "file with such name already exists");

  // create inode and block for new file
  uint16_t inode_id = occupy_inode(&sb);
  uint16_t block_id = occupy_block(&sb);

  struct inode inode;
  reset_inode(&inode);
  inode.is_file = true;
  inode.block_ids[0] = block_id;

  write_inode(fd, inode_id, &inode);

  // update info of directory
  prev_inode.size += 1;
  write_inode(fd, prev_inode_id, &prev_inode);

  // add new file to records
  reset_dir_record(&record);

  record.inode_id = inode_id;
  strcpy(record.name, file_name);

  write_block(fd, prev_inode.block_ids[0],
              (prev_inode.size - 1) * DIR_RECORD_SIZE, (char*)&record,
              DIR_RECORD_SIZE);

  // write superblock
  write_superblock(fd, &sb);

  close(fd);
}
