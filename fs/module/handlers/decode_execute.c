#include "decode_execute.h"

#include <linux/types.h>
#include <linux/string.h>

#include "../constants/opcodes.h"
#include "../server_utils/fs_types.h"
#include "../server_utils/module_defines.h"
#include "handlers.h"

#define FS_FILE "/root/fs.bin"

int decode_execute(const char* data, char* results) {
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

      return init_fs(results, FS_FILE);

    case FS_DESTROY:
      server_log1("command: destroy");

      return destroy_fs(results, FS_FILE);

    case FS_LS:
      server_log1("command: ls");

      str = data + sizeof(enum OPCODE);

      return list_dir(results, FS_FILE, str);

    case FS_MKDIR:
      server_log1("command: mkdir");

      str = data + sizeof(enum OPCODE);

      return create_dir(results, FS_FILE, str);

    case FS_RM:
      server_log1("command: rm");

      str = data + sizeof(enum OPCODE);

      return delete_object(results, FS_FILE, str);

    case FS_TOUCH:
      server_log1("command: touch");

      str = data + sizeof(enum OPCODE);

      return create_file(results, FS_FILE, str);

    case FS_OPEN:
      server_log1("command: open");

      str = data + sizeof(enum OPCODE);

      return open_file(results, FS_FILE, str);

    case FS_CLOSE:
      server_log1("command: close");

      memcpy(&file_descr, data + sizeof(enum OPCODE), sizeof(__u16));

      return close_file(results, FS_FILE, file_descr);

    case FS_SEEK:
      server_log1("command: seek");

      memcpy(&file_descr, data + sizeof(enum OPCODE), sizeof(__u16));
      memcpy(&pos, data + sizeof(enum OPCODE) + sizeof(__u16), sizeof(__u32));

      return seek_pos(results, FS_FILE, file_descr, pos);

    case FS_WRITE:
      server_log1("command: write");

      memcpy(&file_descr, data + sizeof(enum OPCODE), sizeof(__u16));
      str = data + sizeof(enum OPCODE) + sizeof(__u16);
      memcpy(&pos, str + strlen(str) + sizeof(enum OPCODE), sizeof(__u32));

      return write_to_file(results, FS_FILE, file_descr, str, pos);

    case FS_READ:
      server_log1("command: read");

      memcpy(&file_descr, data + sizeof(enum OPCODE), sizeof(__u16));
      memcpy(&pos, data + sizeof(enum OPCODE) + sizeof(__u16), sizeof(__u32));

      return read_from_file(results, FS_FILE, file_descr, pos);
  }
}
