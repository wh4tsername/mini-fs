#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <utils.h>

void write_to_file(const char* fs_path,
                   uint16_t file_descr,
                   const char* path,
                   uint32_t size) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
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

    // calculate start offsets
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

    reset_inode(&cur_inode);
    read_inode(fd, cur_inode_id, &cur_inode);
    for (uint16_t i = 1; i <= start_layer; ++i) {
        // update prev inode sizes
        cur_inode.size += size;
        write_inode(fd, cur_inode_id, &cur_inode);

//        reset_inode(&cur_inode);
//        read_inode(fd, cur_inode_id, &cur_inode);

        prev_inode_id = cur_inode_id;
        cur_inode_id = cur_inode.inode_id;

        reset_inode(&cur_inode);
        read_inode(fd, cur_inode_id, &cur_inode);
    }

//    // lazy inode allocation
//    if (cur_inode_id == 0) {
//        struct inode new_inode;
//        reset_inode(&new_inode);
//        new_inode.is_file = true;
//        new_inode.size = 0;
//
//        uint16_t new_inode_id = occupy_inode(&sb);
//
//        // get prev and update
//        struct inode prev_inode;
//        reset_inode(&prev_inode);
//
//        read_inode(fd, prev_inode_id, &prev_inode);
//        prev_inode.inode_id = new_inode_id;
//
//        // update cur_inode_id
//        cur_inode_id = new_inode_id;
//
//        // update prev
//        write_inode(fd, prev_inode_id, &prev_inode);
//
//        // write new inode
//        write_inode(fd, new_inode_id, &new_inode);
//
//        // get new inode to cur_inode
//        read_inode(fd, new_inode_id, &cur_inode);
//    }

    // read data from file to buffer
    read_retry(source_fd, buffer, size);

    // write recursively
    uint32_t buffer_pos = 0;
    uint32_t left_size = size;
    uint32_t cur_written = 0;
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

        // includes right bound
        uint16_t left_block = 0;
        uint16_t right_block = 0;
        if (i == 0) {
            left_block = start_block_num;
        } else {
            left_block = 0;
        }

        if (i == fin_layer - start_layer) {
            right_block = fin_block_num;
        } else {
            right_block = NUM_BLOCK_IDS_IN_INODE - 1;
        }

        // iterate blocks
        for (uint16_t j = left_block; j <= right_block; ++j) {
            // lazy block allocation
            if (cur_inode.block_ids[j] == 0) {
                uint16_t new_block_id = occupy_block(&sb);

                printf("\nnew: %d\n", new_block_id);

                cur_inode.block_ids[j] = new_block_id;
            }

            // doesn't include right bound
            uint16_t left_block_shift = 0;
            uint16_t right_block_shift = 0;
            if (j == start_block_num && i == 0) {
                left_block_shift = start_block_shift;
            } else {
                left_block_shift = 0;
            }

            if (j == fin_block_num && i == fin_layer - start_layer) {
                right_block_shift = fin_block_shift;
            } else {
                right_block_shift = BLOCK_SIZE;
            }

            printf("\nwrite bounds: %d %d %d %d\n",
                   cur_inode_id,
                   cur_inode.block_ids[j],
                   left_block_shift,
                   right_block_shift);

            write_block(fd,
                        cur_inode.block_ids[j],
                        left_block_shift,
                        buffer + buffer_pos,
                        right_block_shift - left_block_shift);

            buffer_pos += right_block_shift - left_block_shift;
            cur_written += right_block_shift - left_block_shift;
        }

        cur_inode.size += left_size;

        conditional_handle_error(
            left_size < cur_written,
            "size error while writing");

        left_size -= cur_written;
        cur_written = 0;

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
