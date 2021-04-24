#ifndef MINI_FS_UTILS_HEADERS_NET_UTILS_H_
#define MINI_FS_UTILS_HEADERS_NET_UTILS_H_

#include <constants/opcodes.h>
#include <stdint.h>

void send_opcode(int sockd, enum OPCODE op);
void send_string(int sockd, const char* string);
void send_uint16_t(int sockd, uint16_t number);
void send_uint32_t(int sockd, uint32_t number);

void recv_opcode(int sockd, enum OPCODE* op);
void recv_string(int sockd, char** string);
void recv_uint16_t(int sockd, uint16_t* number);
void recv_uint32_t(int sockd, uint32_t* number);

#endif //MINI_FS_UTILS_HEADERS_NET_UTILS_H_
