#include "superblock.h"

#include <defines.h>

void reset_superblock(struct superblock* sb_pointer) {
    for (uint16_t i = 0; i < NUM_BLOCKS; ++i) {
        sb_pointer->occupied_blocks_mask[i] = false;
    }
    for (uint16_t i = 0; i < NUM_INODES; ++i) {
        sb_pointer->occupied_inodes_mask[i] = false;
    }
}

uint16_t occupy_block(struct superblock* sb_pointer) {
    for (uint16_t i = 0; i < NUM_BLOCKS; ++i) {
        if (sb_pointer->occupied_blocks_mask[i] == false) {
            sb_pointer->occupied_blocks_mask[i] = true;
            return i + 1;
        }
    }

    panic("no empty blocks to occupy");
}

void free_block(struct superblock* sb_pointer, uint16_t block_id) {
    conditional_handle_error(sb_pointer->occupied_blocks_mask[block_id - 1] == false,
                             "trying to double free block");

    sb_pointer->occupied_blocks_mask[block_id - 1] = false;
}

uint16_t occupy_inode(struct superblock* sb_pointer) {
    for (uint16_t i = 0; i < NUM_INODES; ++i) {
        if (sb_pointer->occupied_inodes_mask[i] == false) {
            sb_pointer->occupied_inodes_mask[i] = true;
            return i + 1;
        }
    }

    panic("no empty inodes to occupy");
}

void free_inode(struct superblock* sb_pointer, uint16_t inode_id) {
    conditional_handle_error(sb_pointer->occupied_inodes_mask[inode_id - 1] == false,
                             "trying to double free inode");

    sb_pointer->occupied_inodes_mask[inode_id - 1] = false;
}
