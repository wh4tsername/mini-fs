#include "descriptor_table.h"

#include <defines.h>

void reset_descriptor_table(struct descriptor_table* dt) {
    for (uint16_t i = 0; i < NUM_DESCRIPTORS; ++i) {
        dt->fd_mask[i] = false;
        dt->inode_id[i] = 0;
        dt->pos[i] = 0;
    }
}

uint16_t occupy_descriptor(
        struct descriptor_table* dt,
        uint16_t inode_id) {
    for (uint16_t i = 0; i < NUM_DESCRIPTORS; ++i) {
        if (dt->inode_id[i] == inode_id) {
            panic("can't open same file twice");
        }
    }

    for (uint16_t i = 0; i < NUM_DESCRIPTORS; ++i) {
        if (dt->fd_mask[i] == false) {
            dt->fd_mask[i] = true;
            dt->inode_id[i] = inode_id;
            dt->pos[i] = 0;
            return i;
        }
    }

    panic("no empty file descriptors to occupy");
}

void free_descriptor(struct  descriptor_table* dt, uint16_t descr) {
    conditional_handle_error(dt->fd_mask[descr] == false,
                             "trying to double close file descriptor");

    dt->fd_mask[descr] = false;
    dt->inode_id[descr] = 0;
    dt->pos[descr] = 0;
}

void change_pos(struct descriptor_table* dt,
                uint16_t descr,
                uint32_t pos) {
    conditional_handle_error(dt->fd_mask[descr] == false,
                             "trying to operate with closed file descriptor");

    dt->pos[descr] = pos;
}
