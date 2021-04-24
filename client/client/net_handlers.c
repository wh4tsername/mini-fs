#include "net_handlers.h"

#include <constants/opcodes.h>
#include <net_utils.h>

void send_init_fs(int sockd) {
  send_opcode(sockd, FS_INIT);
}

void send_destroy_fs(int sockd) {
  send_opcode(sockd, FS_DESTROY);
}

void send_list_dir(int sockd, const char* path) {
  send_opcode(sockd, FS_LS);

  send_string(sockd, path);
}

void send_create_dir(int sockd, const char* path) {
  send_opcode(sockd, FS_MKDIR);

  send_string(sockd, path);
}

void send_delete_object(int sockd, const char* path) {
  send_opcode(sockd, FS_RM);

  send_string(sockd, path);
}

void send_create_file(int sockd, const char* path) {
  send_opcode(sockd, FS_TOUCH);

  send_string(sockd, path);
}

void send_open_file(int sockd, const char* path) {
  send_opcode(sockd, FS_OPEN);

  send_string(sockd, path);
}

void send_close_file(int sockd, uint16_t file_descr) {
  send_opcode(sockd, FS_CLOSE);

  send_uint16_t(sockd, file_descr);
}

void send_seek_pos(int sockd, uint16_t file_descr, uint32_t pos) {
  send_opcode(sockd, FS_SEEK);

  send_uint16_t(sockd, file_descr);
  send_uint32_t(sockd, pos);
}

void send_write_to_file(int sockd, uint16_t file_descr,
                        const char* path, uint32_t size) {
  send_opcode(sockd, FS_WRITE);

  send_uint16_t(sockd, file_descr);
  send_string(sockd, path);
  send_uint32_t(sockd, size);
}

void send_read_from_file(int sockd, uint16_t file_descr,
                         const char* path, uint32_t size) {
  send_opcode(sockd, FS_READ);

  send_uint16_t(sockd, file_descr);
  send_string(sockd, path);
  send_uint32_t(sockd, size);
}

void send_quit(int sockd) {
  send_opcode(sockd, FS_QUIT);
}
