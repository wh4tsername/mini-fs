#include <linux/string.h>

#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int list_dir(char* results, char* memory, const char* path) {
  struct fs_inode inode;
  check_ret_code(traverse_path(memory, path, &inode));

  __u16 block_id = inode.block_ids[0];

  struct dir_record records[16];
  check_ret_code(read_dir_records(memory, block_id, records, inode.size));

  int num_strings = 1;
  memcpy(results, &num_strings, sizeof(char));
  results += sizeof(char);
  memcpy(results, &inode.size, sizeof(unsigned char));
  results += sizeof(unsigned char);
  for (__u32 i = 0; i < inode.size; ++i) {
    struct fs_inode obj_inode;
    check_ret_code(reset_inode(&obj_inode));
    check_ret_code(read_inode(memory, records[i].inode_id, &obj_inode));

    size_t len = strlen(records[i].name);
    memcpy(results, records[i].name, len + 1);
    results += len + 1;
    if (obj_inode.is_file) {
      const char* file_string = " - file";
      int file_string_len = 7;

      memcpy(results - 1, file_string, file_string_len + 1);
    }
  }

  return 0;
}
