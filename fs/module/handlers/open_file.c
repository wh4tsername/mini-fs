#include <linux/string.h>

#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int check_for_file(char* memory, struct fs_inode* inode, __u16 block_id,
                   const char* name, __u16* inode_id, bool* file_exists) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  check_ret_code(read_dir_records(memory, block_id, records, inode->size));

  for (__u32 i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      *inode_id = records[i].inode_id;
      *file_exists = true;

      return 0;
    }
  }

  *file_exists = false;

  return 0;
}

int open_file(char* results, char* memory, const char* path) {
  char path_to_traverse[MAX_PATH_LENGTH];
  char file_name[MAX_PATH_LENGTH];
  check_ret_code(split_path(path, path_to_traverse, file_name));

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

  // get inode_id of file
  __u16 inode_id = 0;

  bool file_exists = false;
  check_ret_code(check_for_file(memory, &prev_inode, prev_inode.block_ids[0],
                                file_name, &inode_id, &file_exists));
  cond_server_panic(!file_exists, "file with such name doesn't exist");
  struct fs_inode file_inode;
  check_ret_code(reset_inode(&file_inode));
  check_ret_code(read_inode(memory, inode_id, &file_inode));

  cond_server_panic(!file_inode.is_file, "can open only files");

  // get descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));

  check_ret_code(read_descriptor_table(memory, &dt));

  // occupy file descriptor
  __u16 descr = occupy_descriptor(&dt, inode_id);
  //  dprintf(output_fd, "%u", (unsigned char)1);
  //  dprintf(output_fd, "fd: %hu\n", descr);

  // write descriptor table
  write_descriptor_table(memory, &dt);

  return 0;
}
