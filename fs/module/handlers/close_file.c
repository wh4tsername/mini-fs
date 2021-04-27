#include "../server_utils/helpers.h"

#include "../server_utils/disk_utils.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/module.h>
#include <linux/fs.h>

int close_file(char* results, const char* fs_path, __u16 file_descr) {
  struct file* f = filp_open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
  cond_server_panic(f == NULL, "open error");

  // get descriptor table
  struct descriptor_table dt;
  reset_descriptor_table(&dt);

  read_descriptor_table(f, &dt);

  // free file descriptor
  free_descriptor(&dt, file_descr);

  // write descriptor table
  write_descriptor_table(f, &dt);

  filp_close(f, NULL);
}
