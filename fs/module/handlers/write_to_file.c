#include "../constants/fs_constants.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

int write_to_file(char* results, char* memory, __u16 file_descr,
                  const char* data, __u32 size) {
  // get superblock
  struct superblock sb;
  check_ret_code(reset_superblock(&sb));
  check_ret_code(read_superblock(memory, &sb));

  // get descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));
  check_ret_code(read_descriptor_table(memory, &dt));

  cond_server_panic(!dt.fd_mask[file_descr],
                    "trying to write to closed file descriptor");

  // calculate start offsets
  __u16 start_layer =
      dt.pos[file_descr] / (NUM_BLOCK_IDS_IN_INODE * FS_BLOCK_SIZE);
  __u16 start_layer_shift =
      dt.pos[file_descr] % (NUM_BLOCK_IDS_IN_INODE * FS_BLOCK_SIZE);

  __u16 start_block_num = start_layer_shift / FS_BLOCK_SIZE;
  __u16 start_block_shift = start_layer_shift % FS_BLOCK_SIZE;

  // calculate final offsets
  __u16 fin_layer =
      (dt.pos[file_descr] + size) / (NUM_BLOCK_IDS_IN_INODE * FS_BLOCK_SIZE);
  __u16 fin_layer_shift =
      (dt.pos[file_descr] + size) % (NUM_BLOCK_IDS_IN_INODE * FS_BLOCK_SIZE);

  __u16 fin_block_num = fin_layer_shift / FS_BLOCK_SIZE;
  __u16 fin_block_shift = fin_layer_shift % FS_BLOCK_SIZE;

  // get start inode_id
  struct fs_inode cur_inode;
  __u16 cur_inode_id = dt.inode_id[file_descr];
  __u16 prev_inode_id = 0;

  check_ret_code(reset_inode(&cur_inode));
  check_ret_code(read_inode(memory, cur_inode_id, &cur_inode));
  for (__u16 i = 1; i <= start_layer; ++i) {
    // update prev inode sizes
    cur_inode.size += size;
    check_ret_code(write_inode(memory, cur_inode_id, &cur_inode));

    prev_inode_id = cur_inode_id;
    cur_inode_id = cur_inode.inode_id;

    check_ret_code(reset_inode(&cur_inode));
    check_ret_code(read_inode(memory, cur_inode_id, &cur_inode));
  }

  // write recursively
  __u32 buffer_pos = 0;
  __u32 left_size = size;
  __u32 cur_written = 0;
  // iterate inodes
  for (__u16 i = 0; i <= fin_layer - start_layer; ++i) {
    // lazy inode allocation
    if (cur_inode_id == 0) {
      struct fs_inode new_inode;
      check_ret_code(reset_inode(&new_inode));
      new_inode.is_file = true;
      new_inode.size = 0;

      __u16 new_inode_id = 0;
      check_ret_code(occupy_inode(&sb, &new_inode_id));

      // cur_inode here already prev
      cur_inode.inode_id = new_inode_id;

      cur_inode_id = new_inode_id;

      // update prev
      check_ret_code(write_inode(memory, prev_inode_id, &cur_inode));

      // write new inode
      check_ret_code(write_inode(memory, new_inode_id, &new_inode));
    }

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
      // lazy block allocation
      if (cur_inode.block_ids[j] == 0) {
        __u16 new_block_id = 0;
        check_ret_code(occupy_block(&sb, &new_block_id));

        cur_inode.block_ids[j] = new_block_id;
      }

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
        right_block_shift = FS_BLOCK_SIZE;
      }

      check_ret_code(write_block(memory, cur_inode.block_ids[j],
                                 left_block_shift, data + buffer_pos,
                                 right_block_shift - left_block_shift));

      buffer_pos += right_block_shift - left_block_shift;
      cur_written += right_block_shift - left_block_shift;
    }

    cur_inode.size += left_size;

    cond_server_panic(left_size < cur_written, "size error while writing");

    left_size -= cur_written;
    cur_written = 0;

    // update cur inode
    check_ret_code(write_inode(memory, cur_inode_id, &cur_inode));

    // get next inode
    prev_inode_id = cur_inode_id;
    cur_inode_id = cur_inode.inode_id;
  }

  // update fd pos
  dt.pos[file_descr] += size;

  // write descriptor table
  check_ret_code(write_descriptor_table(memory, &dt));

  // write superblock
  check_ret_code(write_superblock(memory, &sb));

  return 0;
}
