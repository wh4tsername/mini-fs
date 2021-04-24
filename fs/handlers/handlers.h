#ifndef MINI_FS__HANDLERS_H_
#define MINI_FS__HANDLERS_H_

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void init_fs(int output_fd, const char* fs_path);

void destroy_fs(int output_fd, const char* fs_path);

void list_dir(int output_fd, const char* fs_path, const char* path);

void create_dir(int output_fd, const char* fs_path, const char* path);

void delete_object(int output_fd, const char* fs_path, const char* path);

void create_file(int output_fd, const char* fs_path, const char* path);

void open_file(int output_fd, const char* fs_path, const char* path);

void close_file(int output_fd, const char* fs_path, uint16_t file_descr);

void seek_pos(int output_fd, const char* fs_path, uint16_t file_descr,
              uint32_t pos);

void write_to_file(int output_fd, const char* fs_path, uint16_t file_descr,
                   const char* path, uint32_t size);

void read_from_file(int output_fd, const char* fs_path, uint16_t file_descr,
                    uint32_t size);

#endif  // MINI_FS__HANDLERS_H_
