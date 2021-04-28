#include "decode_execute.h"

#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include "../constants/opcodes.h"
#include "../server_utils/fs_types.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

int decode_execute(char* memory, const char* data, char* results) {
  enum OPCODE op = (unsigned char)data[0];
  const char* str = NULL;
  __u16 file_descr = 0;
  __u32 pos = 0;

  switch (op) {
    case FS_QUIT:
      server_log1("command: quit");

      return -1;

    case FS_INIT:
      server_log1("command: init");

      return init_fs(results, memory);

    case FS_DESTROY:
      server_log1("command: destroy");

      return destroy_fs(results, memory);

    case FS_LS:
      server_log1("command: ls");

      str = data + 1;

      return list_dir(results, memory, str);

    case FS_MKDIR:
      server_log1("command: mkdir");

      str = data + 1;

      return create_dir(results, memory, str);

    case FS_RM:
      server_log1("command: rm");

      str = data + 1;

      return delete_object(results, memory, str);

    case FS_TOUCH:
      server_log1("command: touch");

//      printk(KERN_INFO "data: %s", data);

      str = data + 1;

//      printk(KERN_INFO "str: %d", 1);

      return create_file(results, memory, str);

    case FS_OPEN:
      server_log1("command: open");

      str = data + 1;

      return open_file(results, memory, str);

    case FS_CLOSE:
      server_log1("command: close");

      memcpy(&file_descr, data + 1, sizeof(__u16));

      return close_file(results, memory, file_descr);

    case FS_SEEK:
      server_log1("command: seek");

      memcpy(&file_descr, data + 1, sizeof(__u16));
      memcpy(&pos, data + 1 + sizeof(__u16), sizeof(__u32));

      return seek_pos(results, memory, file_descr, pos);

    case FS_WRITE:
      server_log1("command: write");

      memcpy(&file_descr, data + 1, sizeof(__u16));
      str = data + 1 + sizeof(__u16);
      memcpy(&pos, str + strlen(str) + 1, sizeof(__u32));

      return write_to_file(results, memory, file_descr, str, pos);

    case FS_READ:
      server_log1("command: read");

      memcpy(&file_descr, data + 1, sizeof(__u16));
      memcpy(&pos, data + 1 + sizeof(__u16), sizeof(__u32));

      return read_from_file(results, memory, file_descr, pos);

    default:
      return -1;
  }
}
