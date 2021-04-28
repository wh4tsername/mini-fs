#include "descriptor_table.h"

#include "../server_utils/module_defines.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int reset_descriptor_table(struct descriptor_table* dt) {
  for (__u16 i = 0; i < NUM_DESCRIPTORS; ++i) {
    dt->fd_mask[i] = false;
    dt->inode_id[i] = 0;
    dt->pos[i] = 0;
  }

  return 0;
}

int occupy_descriptor(struct descriptor_table* dt, __u16 inode_id,
                      __u16* file_descr) {
  for (__u16 i = 0; i < NUM_DESCRIPTORS; ++i) {
    if (dt->inode_id[i] == inode_id) {
      server_panic("can't open same file twice");
    }
  }

  for (__u16 i = 0; i < NUM_DESCRIPTORS; ++i) {
    if (dt->fd_mask[i] == false) {
      dt->fd_mask[i] = true;
      dt->inode_id[i] = inode_id;
      dt->pos[i] = 0;

      *file_descr = i;

      return 0;
    }
  }

  server_panic("no empty file descriptors to occupy");
}

int free_descriptor(struct descriptor_table* dt, __u16 descr) {
  cond_server_panic(dt->fd_mask[descr] == false,
                    "trying to double close file descriptor");

  dt->fd_mask[descr] = false;
  dt->inode_id[descr] = 0;
  dt->pos[descr] = 0;

  return 0;
}

int change_pos(struct descriptor_table* dt, __u16 descr, __u32 pos) {
  cond_server_panic(dt->fd_mask[descr] == false,
                    "trying to operate with closed file descriptor");

  dt->pos[descr] = pos;

  return 0;
}
