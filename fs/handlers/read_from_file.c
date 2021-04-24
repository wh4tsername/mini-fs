#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <disk_utils.h>

void read_from_file(const char* fs_path,
                    uint16_t file_descr,
                    const char* path,
                    uint32_t size) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    int dest_fd = open(path, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    conditional_parse_errno(dest_fd == -1);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);
    read_descriptor_table(fd, &dt);

    conditional_handle_error(!dt.fd_mask[file_descr],
                             "trying to read from closed file descriptor");

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

    reset_inode(&cur_inode);
    read_inode(fd, cur_inode_id, &cur_inode);
    for (uint16_t i = 1; i <= start_layer; ++i) {
        cur_inode_id = cur_inode.inode_id;

        reset_inode(&cur_inode);
        read_inode(fd, cur_inode_id, &cur_inode);
    }

    // check if read in bounds
    conditional_handle_error(
        (uint64_t)size > (uint64_t)cur_inode.size - (uint64_t)start_layer_shift,
        "read length is out of bounds");
    conditional_handle_error(
        (uint64_t)fin_layer_shift +
        (uint64_t)(fin_layer - start_layer) *
        (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE) > (uint64_t)cur_inode.size,
        "read length is out of bounds");

    // read recursively
    char* buffer = malloc(BLOCK_SIZE);
    for (uint16_t i = 0; i <= fin_layer - start_layer; ++i) {
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

            read_block(fd,
                       cur_inode.block_ids[j],
                       left_block_shift,
                       buffer,
                       right_block_shift - left_block_shift);

            write_retry(dest_fd,
                        buffer,
                        right_block_shift - left_block_shift);
        }

        cur_inode_id = cur_inode.inode_id;
    }

    // update fd pos
    dt.pos[file_descr] += size;

    // write descriptor table
    write_descriptor_table(fd, &dt);

    free(buffer);

    close(dest_fd);

    close(fd);
}
