#include "../server_utils/helpers.h"

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/fs.h>
#include <linux/string.h>

int list_dir(char* results, const char* fs_path, const char* path) {
  struct file* f = filp_open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(f == NULL, "open error");

  struct fs_inode inode;
  check_ret_code(traverse_path(f, path, &inode));

  __u16 block_id = inode.block_ids[0];

  struct dir_record records[16];
  read_dir_records(f, block_id, records, inode.size);

  int num_strings = 1;
  memcpy(results, &num_strings, sizeof(char));
  results += sizeof(char);
  memcpy(results, &inode.size, sizeof(unsigned char));
  results += sizeof(unsigned char);
  for (__u16 i = 0; i < inode.size; ++i) {
    struct fs_inode obj_inode;
    reset_inode(&obj_inode);
    read_inode(f, records[i].inode_id, &obj_inode);

    size_t len = strlen(records[i].name);
    memcpy(results, records[i].name, len + 1);
    results += len + 1;
    if (obj_inode.is_file) {
      const char* file_string = " - file";
      int file_string_len = 7;

      memcpy(results - 1, file_string, file_string_len + 1);
    }
  }

  filp_close(f, NULL);
}
