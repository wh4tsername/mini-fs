#ifndef MINI_FS_CLIENT_NET_HANDLERS_H_
#define MINI_FS_CLIENT_NET_HANDLERS_H_

#include <stdint.h>

void send_init_fs(int sockd);

void send_destroy_fs(int sockd);

void send_list_dir(int sockd, const char* path);

void send_create_dir(int sockd, const char* path);

void send_delete_object(int sockd, const char* path);

void send_create_file(int sockd, const char* path);

void send_open_file(int sockd, const char* path);

void send_close_file(int sockd, uint16_t file_descr);

void send_seek_pos(int sockd, uint16_t file_descr, uint32_t pos);

void send_write_to_file(int sockd, uint16_t file_descr,
                        const char* path, uint32_t size);

void send_read_from_file(int sockd, uint16_t file_descr,
                         const char* path, uint32_t size);

void send_quit(int sockd);

#endif  // MINI_FS_CLIENT_NET_HANDLERS_H_
