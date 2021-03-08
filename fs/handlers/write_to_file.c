#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <utils.h>

void write_to_file(uint16_t file_descr, const char* path, uint32_t size) {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    char* buffer = malloc(size);

    int source_fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
    conditional_parse_errno(source_fd == -1);

    // get superblock
    struct superblock sb;
    reset_superblock(&sb);
    read_superblock(fd, &sb);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    read_descriptor_table(fd, &dt);

    conditional_handle_error(!dt.fd_mask[file_descr],
                             "trying to write to closed file descriptor");

    // calculate cur offsets
    uint16_t start_layer =
        dt.pos[file_descr] / (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);
    uint16_t start_layer_shift =
        dt.pos[file_descr] % (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);

    uint16_t start_block_num = start_layer_shift / BLOCK_SIZE;
    uint16_t start_block_shift = start_layer_shift % BLOCK_SIZE;

    // calculate final offsets
    uint16_t fin_layer =
        (dt.pos[file_descr] + size) / (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);
    uint16_t fin_layer_shift =
        (dt.pos[file_descr] + size) % (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);

    uint16_t fin_block_num = fin_layer_shift / BLOCK_SIZE;
    uint16_t fin_block_shift = fin_layer_shift % BLOCK_SIZE;

    // get start inode_id
    struct inode cur_inode;
    uint16_t cur_inode_id = dt.inode_id[file_descr];
    uint16_t prev_inode_id = 0;
    for (uint16_t i = 1; i <= start_layer; ++i) {
        reset_inode(&cur_inode);

        read_inode(fd, cur_inode_id, &cur_inode);

        prev_inode_id = cur_inode_id;
        cur_inode_id = cur_inode.inode_id;
    }

    // write recursively
    uint32_t buffer_pos = 0;
    // iterate inodes
    for (uint16_t i = 0; i <= fin_layer - start_layer; ++i) {
        // lazy inode allocation
        if (cur_inode_id == 0) {
            struct inode new_inode;
            reset_inode(&new_inode);
            new_inode.is_file = true;
            new_inode.size = 0;

            uint16_t new_inode_id = occupy_inode(&sb);

            // cur_inode here already prev
            cur_inode.inode_id = new_inode_id;

            cur_inode_id = new_inode_id;

            // update prev
            write_inode(fd, prev_inode_id, &cur_inode);

            // write new inode
            write_inode(fd, new_inode_id, &new_inode);
        }

        // get inode
        reset_inode(&cur_inode);
        read_inode(fd, cur_inode_id, &cur_inode);

        uint16_t left_block = 0;
        uint16_t right_block = 0;
        if (fin_layer == start_layer) {
            left_block = start_block_num;
            right_block = fin_block_num;
        } else if (i == 0) {
            left_block = start_block_num;
            right_block = NUM_BLOCK_IDS_IN_INODE - 1;
        } else if (i == fin_layer - start_layer) {
            left_block = 0;
            right_block = fin_block_num;
        } else {
            left_block = 0;
            right_block = NUM_BLOCK_IDS_IN_INODE - 1;
        }

        // iterate blocks
        for (uint16_t j = left_block; j <= right_block; ++j) {
            // lazy block allocation
            if (cur_inode.block_ids[j] == 0) {
                uint16_t new_block_id = occupy_block(&sb);

                cur_inode.block_ids[j] = new_block_id;
            }

            uint16_t left_block_shift = 0;
            uint16_t right_block_shift = 0;
            if (left_block == right_block) {
                left_block_shift = start_block_shift;
                right_block_shift = fin_block_shift;
            } else if (j == left_block) {
                left_block_shift = start_block_shift;
                right_block_shift = BLOCK_SIZE;
            } else if (j == right_block) {
                left_block_shift = 0;
                right_block_shift = fin_block_shift;
            } else {
                left_block_shift = 0;
                right_block_shift = BLOCK_SIZE;
            }

            write_block(fd,
                        cur_inode.block_ids[j],
                        left_block_shift,
                        buffer + buffer_pos,
                        right_block_shift - left_block_shift);

            buffer_pos += right_block_shift - left_block_shift;
        }

        cur_inode.size += size - i * NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE;
        conditional_handle_error(size < i * NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE,
                                 "size error while writing");

        // update cur inode
        write_inode(fd, cur_inode_id, &cur_inode);

        // get next inode
        prev_inode_id = cur_inode_id;
        cur_inode_id = cur_inode.inode_id;
    }

    // update fd pos
    dt.pos[file_descr] += size;

    // write descriptor table
    write_descriptor_table(fd, &dt);

    // write superblock
    write_superblock(fd, &sb);

    free(buffer);

    close(source_fd);

    close(fd);
}
