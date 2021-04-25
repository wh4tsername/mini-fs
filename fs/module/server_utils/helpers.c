#include "helpers.h"
#include <string.h>

#include "string_utils.h"
#include "disk_utils.h"
#include "module_defines.h"

uint16_t create_dir_block_and_inode(int fd, struct superblock* sb, bool is_root,
                                    uint16_t prev_inode_id) {
  uint16_t block_id = occupy_block(sb);
  uint16_t inode_id = occupy_inode(sb);

  // create inode
  struct fs_inode inode;
  reset_inode(&inode);
  inode.block_ids[0] = block_id;
  inode.is_file = false;
  inode.size = 2;

  // create dir_records
  struct dir_record records[2];
  reset_dir_record(&records[0]);
  reset_dir_record(&records[1]);
  records[0].inode_id = inode_id;
  records[1].inode_id = is_root ? inode_id : prev_inode_id;
  strcpy(records[0].name, ".");
  strcpy(records[1].name, "..");

  write_dir_records(fd, block_id, records, inode.size);
  write_inode(fd, inode_id, &inode);

  return inode_id;
}

// traverse recursive
void traverse_recursively(int fd, const char* path, struct fs_inode* current,
                          struct fs_inode* res) {
  cond_server_panic(current->is_file, "trying to enter file");

  char current_layer[MAX_PATH_LENGTH];
  char* remaining_path = parse_path(path, current_layer);

  if (strcmp(current_layer, "/") == 0) {
    memcpy(res, current, sizeof(struct fs_inode));
  } else {
    struct dir_record records[16];

    read_dir_records(fd, current->block_ids[0], records, current->size);

    bool dir_found = false;
    uint16_t i = 0;
    for (; i < current->size; ++i) {
      if (strcmp(records[i].name, current_layer) == 0) {
        dir_found = true;

        break;
      }
    }

    cond_server_panic(!dir_found, "directory doesn't exist");

    struct fs_inode next;
    read_inode(fd, records[i].inode_id, &next);

    cond_server_panic(next.is_file, "trying to enter file");

    if (remaining_path == NULL) {
      memcpy(res, &next, sizeof(struct fs_inode));
    } else {
      traverse_recursively(fd, remaining_path, &next, res);
    }
  }
}

void traverse_path(int fd, const char* path, struct fs_inode* res) {
  // inode of "/" = 1
  struct fs_inode cur_inode;
  read_inode(fd, 1, &cur_inode);

  traverse_recursively(fd, path, &cur_inode, res);
}

char* parse_path(const char* path, char* next_token) {
  char* slash_pos = strchr(path, '/');
  cond_server_panic(slash_pos == NULL, "incorrect path");

  uint16_t path_length = strlen(path);

  // path = "/"
  if (path_length == 1) {
    strcpy(next_token, "/");
    return NULL;
  }

  char* next_slash_pos = strchr(slash_pos + 1, '/');
  uint16_t next_token_length = 0;
  if (next_slash_pos == NULL) {
    next_token_length = path + path_length - slash_pos - 1;
  } else {
    next_token_length = next_slash_pos - slash_pos - 1;

    cond_server_panic(next_token_length == 0, "incorrect path");
  }

  memcpy(next_token, slash_pos + 1, next_token_length);
  next_token[next_token_length] = '\0';

  return next_slash_pos;
}

void split_path(const char* path, char* path_to_traverse, char* dir_name) {
  uint16_t path_length = strlen(path);
  char buffer[MAX_PATH_LENGTH];
  delete_last_slash_and_copy_res(path, path_length, buffer);

  uint16_t buffer_length = strlen(buffer);
  cond_server_panic(path[0] != '/' || buffer_length == 0,
                           "incorrect path");

  int32_t last_slash_pos = find_last_occurrence(buffer, '/');
  memcpy(dir_name, buffer + last_slash_pos + 1,
         buffer_length - last_slash_pos - 1);
  dir_name[buffer_length - last_slash_pos - 1] = '\0';

  cond_server_panic(strlen(dir_name) == 0, "incorrect path");

  memcpy(path_to_traverse, buffer, last_slash_pos + 1);
  path_to_traverse[last_slash_pos + 1] = '\0';
}
