#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int read_from_file(char* results, char* memory, __u16 file_descr,
                    __u32 size) {
  // get descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));
  check_ret_code(read_descriptor_table(memory, &dt));

  cond_server_panic(!dt.fd_mask[file_descr],
                    "trying to read from closed file descriptor");

  // calculate start offsets
  __u16 start_layer =
      dt.pos[file_descr] / (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);
  __u16 start_layer_shift =
      dt.pos[file_descr] % (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);

  __u16 start_block_num = start_layer_shift / BLOCK_SIZE;
  __u16 start_block_shift = start_layer_shift % BLOCK_SIZE;

  // calculate final offsets
  __u16 fin_layer =
      (dt.pos[file_descr] + size) / (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);
  __u16 fin_layer_shift =
      (dt.pos[file_descr] + size) % (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE);

  __u16 fin_block_num = fin_layer_shift / BLOCK_SIZE;
  __u16 fin_block_shift = fin_layer_shift % BLOCK_SIZE;

  // get start inode_id
  struct fs_inode cur_inode;
  __u16 cur_inode_id = dt.inode_id[file_descr];

  check_ret_code(reset_inode(&cur_inode));
  check_ret_code(read_inode(memory, cur_inode_id, &cur_inode));
  for (__u16 i = 1; i <= start_layer; ++i) {
    cur_inode_id = cur_inode.inode_id;

    check_ret_code(reset_inode(&cur_inode));
    check_ret_code(read_inode(memory, cur_inode_id, &cur_inode));
  }

  // check if read in bounds
  cond_server_panic(
      (__u64)size > (__u64)cur_inode.size - (__u64)start_layer_shift,
      "read length is out of bounds");
  cond_server_panic(
      (__u64)fin_layer_shift + (__u64)(fin_layer - start_layer) *
                                      (NUM_BLOCK_IDS_IN_INODE * BLOCK_SIZE) >
          (__u64)cur_inode.size,
      "read length is out of bounds");

  // read recursively
  for (__u16 i = 0; i <= fin_layer - start_layer; ++i) {
    // get inode
    check_ret_code(reset_inode(&cur_inode));
    check_ret_code(read_inode(memory, cur_inode_id, &cur_inode));

    // includes right bound
    __u16 left_block = 0;
    __u16 right_block = 0;
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
    for (__u16 j = left_block; j <= right_block; ++j) {
      // doesn't include right bound
      __u16 left_block_shift = 0;
      __u16 right_block_shift = 0;
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

      check_ret_code(read_block(memory, cur_inode.block_ids[j], left_block_shift, results,
                 right_block_shift - left_block_shift));
      int res_size = right_block_shift - left_block_shift;
    }

    cur_inode_id = cur_inode.inode_id;
  }

  // update fd pos
  dt.pos[file_descr] += size;

  // write descriptor table
  check_ret_code(write_descriptor_table(memory, &dt));

  return 0;
}
