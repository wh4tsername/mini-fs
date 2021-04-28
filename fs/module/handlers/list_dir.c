#include <linux/kernel.h>
#include <linux/string.h>

#include "../server_utils/disk_utils.h"
#include "../server_utils/helpers.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int list_dir(char* results, char* memory, const char* path) {
  struct fs_inode inode;
  check_ret_code(traverse_path(memory, path, &inode));

  __u16 block_id = inode.block_ids[0];

  struct dir_record records[16];
  check_ret_code(read_dir_records(memory, block_id, records, inode.size));

  sprintf(results, "%u", (unsigned char)inode.size);
  for (__u32 i = 0; i < inode.size; ++i) {
    struct fs_inode obj_inode;
    check_ret_code(reset_inode(&obj_inode));
    check_ret_code(read_inode(memory, records[i].inode_id, &obj_inode));

    sprintf(results, (obj_inode.is_file ? "%s - file\n" : "%s\n"),
            records[i].name);
  }

  return 0;
}
