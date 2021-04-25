#include "net_utils.h"

#include <defines.h>
#include <io_utils.h>
#include <string.h>

void send_opcode(int sockd, enum OPCODE op) {
  ssize_t written = write_retry(sockd, (char*)&op, sizeof(enum OPCODE));
  conditional_handle_error(written != sizeof(enum OPCODE),
                           "error while sending opcode");
}

void send_string(int sockd, const char* string) {
  size_t length = strlen(string);
  ssize_t written = write_retry(sockd, (char*)&length, sizeof(size_t));
  conditional_handle_error(written != sizeof(size_t),
                           "error while sending string length");

  written = write_retry(sockd, string, (int)length);
  conditional_handle_error(written != length, "error while sending string");
}

void send_uint16_t(int sockd, uint16_t number) {
  ssize_t written = write_retry(sockd, (char*)&number, sizeof(uint16_t));
  conditional_handle_error(written != sizeof(uint16_t),
                           "error while sending uint16_t");
}

void send_uint32_t(int sockd, uint32_t number) {
  ssize_t written = write_retry(sockd, (char*)&number, sizeof(uint32_t));
  conditional_handle_error(written != sizeof(uint32_t),
                           "error while sending uint32_t");
}

void recv_opcode(int sockd, enum OPCODE* op) {
  ssize_t read = read_retry(sockd, (char*)op, sizeof(enum OPCODE));
  conditional_handle_error(read != sizeof(enum OPCODE),
                           "error while receiving opcode");
}

void recv_string(int sockd, char** string) {
  size_t length = 0;
  ssize_t read = read_retry(sockd, (char*)&length, sizeof(size_t));
  conditional_handle_error(read != sizeof(size_t),
                           "error while receiving string length");

  *string = malloc(length + 1);

  read = read_retry(sockd, *string, (int)length);
  conditional_handle_error(read != length, "error while receiving string");
  (*string)[length] = '\0';
}

void recv_uint16_t(int sockd, uint16_t* number) {
  ssize_t read = read_retry(sockd, (char*)number, sizeof(uint16_t));
  conditional_handle_error(read != sizeof(uint16_t),
                           "error while receiving uint16_t");
}

void recv_uint32_t(int sockd, uint32_t* number) {
  ssize_t read = read_retry(sockd, (char*)number, sizeof(uint32_t));
  conditional_handle_error(read != sizeof(uint32_t),
                           "error while receiving uint32_t");
}
