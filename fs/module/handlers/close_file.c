#include "../server_utils/helpers.h"
#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"

#include "handlers.h"

int close_file(char* results, char* memory, __u16 file_descr) {
  // get descriptor table
  struct descriptor_table dt;
  check_ret_code(reset_descriptor_table(&dt));

  check_ret_code(read_descriptor_table(memory, &dt));

  // free file descriptor
  check_ret_code(free_descriptor(&dt, file_descr));

  // write descriptor table
  check_ret_code(write_descriptor_table(memory, &dt));

  return 0;
}
