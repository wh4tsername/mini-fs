#include "helpers.h"

#include "string_utils.h"
#include "disk_utils.h"
#include "module_defines.h"

#include <linux/string.h>

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int create_dir_block_and_inode(char* memory, struct superblock* sb, bool is_root,
                                    __u16 prev_inode_id, __u16* ret_inode_id) {
  __u16 block_id = 0;
  check_ret_code(occupy_block(sb, &block_id));

  __u16 inode_id = 0;
  check_ret_code(occupy_inode(sb, &inode_id));

  // create inode
  struct fs_inode inode;
  check_ret_code(reset_inode(&inode));
  inode.block_ids[0] = block_id;
  inode.is_file = false;
  inode.size = 2;

  // create dir_records
  struct dir_record records[2];
  check_ret_code(reset_dir_record(&records[0]));
  check_ret_code(reset_dir_record(&records[1]));
  records[0].inode_id = inode_id;
  records[1].inode_id = is_root ? inode_id : prev_inode_id;
  strcpy(records[0].name, ".");
  strcpy(records[1].name, "..");

  check_ret_code(write_dir_records(memory, block_id, records, inode.size));
  check_ret_code(write_inode(memory, inode_id, &inode));

  *ret_inode_id = inode_id;

  return 0;
}

// traverse recursive
int traverse_recursively(char* memory, const char* path, struct fs_inode* current_inode,
                         struct fs_inode* res) {
  cond_server_panic(current_inode->is_file, "trying to enter file");

  char current_layer[MAX_PATH_LENGTH];
  char* remaining_path = NULL;
  check_ret_code(parse_path(path, current_layer, &remaining_path));

  if (strcmp(current_layer, "/") == 0) {
    memcpy(res, current_inode, sizeof(struct fs_inode));
  } else {
    struct dir_record records[16];

    check_ret_code(read_dir_records(memory,
                                    current_inode->block_ids[0],
                                    records,
                                    current_inode->size));

    bool dir_found = false;
    __u32 i = 0;
    for (; i < current_inode->size; ++i) {
      if (strcmp(records[i].name, current_layer) == 0) {
        dir_found = true;

        break;
      }
    }

    cond_server_panic(!dir_found, "directory doesn't exist");

    struct fs_inode next;
    check_ret_code(read_inode(memory, records[i].inode_id, &next));

    cond_server_panic(next.is_file, "trying to enter file");

    if (remaining_path == NULL) {
      memcpy(res, &next, sizeof(struct fs_inode));
    } else {
      int ret = traverse_recursively(memory, remaining_path, &next, res);

      if (ret < 0) {
        return ret;
      }
    }
  }

  return 0;
}

int traverse_path(char* memory, const char* path, struct fs_inode* res) {
  // inode of "/" = 1
  struct fs_inode cur_inode;
  check_ret_code(read_inode(memory, 1, &cur_inode));

  int ret = traverse_recursively(memory, path, &cur_inode, res);

  return ret;
}

int parse_path(const char* path, char* next_token, char** res) {
  char* slash_pos = strchr(path, '/');
  cond_server_panic(slash_pos == NULL, "incorrect path");

  __u16 path_length = strlen(path);

  // path = "/"
  if (path_length == 1) {
    strcpy(next_token, "/");
    *res = NULL;

    return 0;
  }

  char* next_slash_pos = strchr(slash_pos + 1, '/');
  __u16 next_token_length = 0;
  if (next_slash_pos == NULL) {
    next_token_length = path + path_length - slash_pos - 1;
  } else {
    next_token_length = next_slash_pos - slash_pos - 1;

    cond_server_panic(next_token_length == 0, "incorrect path");
  }

  memcpy(next_token, slash_pos + 1, next_token_length);
  next_token[next_token_length] = '\0';

  *res = next_slash_pos;

  return 0;
}

#include <linux/kernel.h>

int split_path(const char* path, char* path_to_traverse, char* dir_name) {
  printk(KERN_INFO "%s", path);

  __u16 path_length = strlen(path);
  char buffer[MAX_PATH_LENGTH];
  check_ret_code(delete_last_slash_and_copy_res(path, path_length, buffer));

  __u16 buffer_length = strlen(buffer);
  cond_server_panic(path[0] != '/' || buffer_length == 0,
                           "incorrect path");

  __s32 last_slash_pos = 0;
  check_ret_code(find_last_occurrence(buffer, '/', &last_slash_pos));

  memcpy(dir_name, buffer + last_slash_pos + 1,
         buffer_length - last_slash_pos - 1);
  dir_name[buffer_length - last_slash_pos - 1] = '\0';

  cond_server_panic(strlen(dir_name) == 0, "incorrect path");

  memcpy(path_to_traverse, buffer, last_slash_pos + 1);
  path_to_traverse[last_slash_pos + 1] = '\0';

  return 0;
}
