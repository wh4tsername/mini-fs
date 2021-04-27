#include <linux/string.h>

#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int check_for_duplicate_dir(char* memory, struct fs_inode* inode,
                            __u16 block_id, const char* name,
                            bool* has_duplicate) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  check_ret_code(read_dir_records(memory, block_id, records, inode->size));

  for (__u32 i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      *has_duplicate = true;

      return 0;
    }
  }

  *has_duplicate = false;

  return 0;
}

int create_dir(char* results, char* memory, const char* path) {
  char path_to_traverse[MAX_PATH_LENGTH];
  char dir_name[MAX_PATH_LENGTH];
  check_ret_code(split_path(path, path_to_traverse, dir_name));

  // get superblock
  struct superblock sb;
  check_ret_code(reset_superblock(&sb));
  check_ret_code(read_superblock(memory, &sb));

  // get inode of prev dir
  struct fs_inode inode;
  check_ret_code(reset_inode(&inode));
  check_ret_code(traverse_path(memory, path_to_traverse, &inode));

  // get block of prev dir
  struct dir_record record;
  check_ret_code(reset_dir_record(&record));
  check_ret_code(read_block(memory, inode.block_ids[0], 0, (char*)&record,
                            DIR_RECORD_SIZE));
  __u16 inode_id = record.inode_id;

  bool has_duplicate = false;
  check_ret_code(check_for_duplicate_dir(memory, &inode, inode.block_ids[0],
                                         dir_name, &has_duplicate));

  cond_server_panic(has_duplicate, "directory with such name already exists");

  __u16 next_inode_id = 0;
  check_ret_code(
      create_dir_block_and_inode(memory, &sb, false, inode_id, &next_inode_id));

  // create dir_record for prev dir
  check_ret_code(reset_dir_record(&record));
  record.inode_id = next_inode_id;
  strcpy(record.name, dir_name);

  // inode of prev dir info update
  cond_server_panic(inode.size >= NUM_RECORDS_IN_DIR, "directory overflow");
  inode.size += 1;
  check_ret_code(write_inode(memory, inode_id, &inode));

  // make new dir_record
  check_ret_code(write_block(memory, inode.block_ids[0],
                             (inode.size - 1) * DIR_RECORD_SIZE,
                             (const char*)&record, DIR_RECORD_SIZE));

  // write superblock
  check_ret_code(write_superblock(memory, &sb));

  return 0;
}
