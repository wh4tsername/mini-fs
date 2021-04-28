#include <linux/string.h>

#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int check_for_duplicate_file(char* memory, struct fs_inode* inode,
                             __u16 block_id, const char* name,
                             bool* file_duplicated) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  check_ret_code(read_dir_records(memory, block_id, records, inode->size));

  for (__u32 i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      *file_duplicated = true;

      return 0;
    }
  }

  *file_duplicated = false;

  return 0;
}

int create_file(char* results, char* memory, const char* path) {
  char path_to_traverse[256];
  char file_name[256];
  check_ret_code(split_path(path, path_to_traverse, file_name));

  // get superblock
  struct superblock sb;
  check_ret_code(reset_superblock(&sb));
  check_ret_code(read_superblock(memory, &sb));

  // get inode of prev dir
  struct fs_inode prev_inode;
  check_ret_code(reset_inode(&prev_inode));
  check_ret_code(traverse_path(memory, path_to_traverse, &prev_inode));

  // get block of prev dir
  struct dir_record record;
  check_ret_code(reset_dir_record(&record));
  check_ret_code(read_block(memory, prev_inode.block_ids[0], 0, (char*)&record,
                            DIR_RECORD_SIZE));
  __u16 prev_inode_id = record.inode_id;

  bool file_duplicated = false;
  check_ret_code(check_for_duplicate_file(memory, &prev_inode,
                                          prev_inode.block_ids[0], file_name,
                                          &file_duplicated));

  cond_server_panic(file_duplicated, "file with such name already exists");

  // create inode and block for new file
  __u16 inode_id = 0;
  check_ret_code(occupy_inode(&sb, &inode_id));
  __u16 block_id = 0;
  check_ret_code(occupy_block(&sb, &inode_id));

  struct fs_inode inode;
  check_ret_code(reset_inode(&inode));
  inode.is_file = true;
  inode.block_ids[0] = block_id;

  check_ret_code(write_inode(memory, inode_id, &inode));

  // update info of directory
  prev_inode.size += 1;
  check_ret_code(write_inode(memory, prev_inode_id, &prev_inode));

  // add new file to records
  check_ret_code(reset_dir_record(&record));

  record.inode_id = inode_id;
  strcpy(record.name, file_name);

  check_ret_code(write_block(memory, prev_inode.block_ids[0],
                             (prev_inode.size - 1) * DIR_RECORD_SIZE,
                             (char*)&record, DIR_RECORD_SIZE));

  // write superblock
  check_ret_code(write_superblock(memory, &sb));

  return 0;
}
