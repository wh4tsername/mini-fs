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

int check_if_obj_exists(char* memory, struct fs_inode* inode, __u16 block_id,
                        const char* name, __u16* res_inode_id,
                        bool* obj_exists) {
  struct dir_record records[NUM_RECORDS_IN_DIR];
  check_ret_code(read_dir_records(memory, block_id, records, inode->size));

  for (__u32 i = 0; i < inode->size; ++i) {
    if (strcmp(name, records[i].name) == 0) {
      *res_inode_id = records[i].inode_id;
      *obj_exists = true;

      return 0;
    }
  }

  *obj_exists = false;

  return 0;
}

// still need to write superblock
int delete_file(char* memory, struct superblock* sb, struct fs_inode* inode,
                __u16 inode_id) {
  // delete children blocks
  for (__u16 i = 0; i < NUM_BLOCK_IDS_IN_INODE; ++i) {
    if (inode->block_ids[i] == 0) {
      break;
    }

    check_ret_code(free_block(sb, inode->block_ids[i]));
    check_ret_code(erase_block(memory, inode->block_ids[i]));
  }

  // delete child inode
  if (inode->inode_id != 0) {
    struct fs_inode next_inode;
    check_ret_code(reset_inode(&next_inode));
    check_ret_code(read_inode(memory, inode->inode_id, &next_inode));

    check_ret_code(delete_file(memory, sb, &next_inode, inode->inode_id));
  }

  // delete myself
  check_ret_code(free_inode(sb, inode_id));
  check_ret_code(erase_inode(memory, inode_id));

  return 0;
}

// still need to write superblock
int delete_directory(char* memory, struct superblock* sb,
                     struct fs_inode* inode, __u16 inode_id) {
  __u16 block_id = inode->block_ids[0];

  struct dir_record records[inode->size];
  check_ret_code(read_dir_records(memory, block_id, records, inode->size));

  // delete children
  for (__u32 i = 2; i < inode->size; ++i) {
    struct fs_inode child_inode;
    check_ret_code(reset_inode(&child_inode));

    check_ret_code(read_inode(memory, records[i].inode_id, &child_inode));

    if (child_inode.is_file) {
      check_ret_code(
          delete_file(memory, sb, &child_inode, records[i].inode_id));
    } else {
      check_ret_code(
          delete_directory(memory, sb, &child_inode, records[i].inode_id));
    }
  }

  // delete myself
  check_ret_code(free_block(sb, block_id));
  check_ret_code(erase_block(memory, block_id));

  check_ret_code(free_inode(sb, inode_id));
  check_ret_code(erase_inode(memory, inode_id));

  return 0;
}

int delete_object(char* results, char* memory, const char* path) {
  char path_to_traverse[MAX_PATH_LENGTH];
  char obj_name[MAX_PATH_LENGTH];
  check_ret_code(split_path(path, path_to_traverse, obj_name));

  cond_server_panic(strcmp(obj_name, ".") == 0 || strcmp(obj_name, "..") == 0,
                    "can't delete \".\" or \"..\"");

  // get superblock
  struct superblock sb;
  check_ret_code(reset_superblock(&sb));
  check_ret_code(read_superblock(memory, &sb));

  // get inode of prev dir
  struct fs_inode prev_inode;
  check_ret_code(reset_inode(&prev_inode));
  check_ret_code(traverse_path(memory, path_to_traverse, &prev_inode));

  // get inode of obj
  __u16 inode_id = 0;
  bool obj_exists = false;
  check_ret_code(check_if_obj_exists(memory, &prev_inode,
                                     prev_inode.block_ids[0], obj_name,
                                     &inode_id, &obj_exists));
  cond_server_panic(!obj_exists,
                    "directory or file with such name doesn't exist");
  struct fs_inode obj_inode;
  check_ret_code(reset_inode(&obj_inode));
  check_ret_code(read_inode(memory, inode_id, &obj_inode));

  // delete obj
  if (obj_inode.is_file) {
    check_ret_code(delete_file(memory, &sb, &obj_inode, inode_id));
  } else {
    check_ret_code(delete_directory(memory, &sb, &obj_inode, inode_id));
  }

  // update parent dir
  __u16 block_id = prev_inode.block_ids[0];

  struct dir_record records[prev_inode.size];
  check_ret_code(read_dir_records(memory, block_id, records, prev_inode.size));

  struct dir_record new_records[prev_inode.size - 1];
  __u32 j = 0;
  for (__u32 i = 0; i < prev_inode.size; ++i) {
    if (strcmp(obj_name, records[i].name) != 0) {
      memcpy((char*)&new_records[j], (char*)&records[i], DIR_RECORD_SIZE);
      ++j;
    }
  }

  // update parent dir block
  check_ret_code(erase_block(memory, block_id));
  check_ret_code(
      write_dir_records(memory, block_id, new_records, prev_inode.size - 1));

  // update parent dir inode
  prev_inode.size -= 1;
  __u16 prev_inode_id = new_records[0].inode_id;

  check_ret_code(erase_inode(memory, prev_inode_id));
  check_ret_code(write_inode(memory, prev_inode_id, &prev_inode));

  // update superblock
  check_ret_code(write_superblock(memory, &sb));

  return 0;
}
