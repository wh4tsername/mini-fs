#ifndef MINI_FS__HANDLERS_H_
#define MINI_FS__HANDLERS_H_

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

void init_fs(const char* fs_path);

void destroy_fs(const char* fs_path);

void list_dir(const char* fs_path, const char* path);

void create_dir(const char* fs_path, const char* path);

void delete_object(const char* fs_path, const char* path);

void create_file(const char* fs_path, const char* path);

void open_file(const char* fs_path, const char* path);

void close_file(const char* fs_path, uint16_t file_descr);

void seek_pos(const char* fs_path, uint16_t file_descr, uint32_t pos);

void write_to_file(const char* fs_path,
                   uint16_t file_descr,
                   const char* path,
                   uint32_t size);

void read_from_file(const char* fs_path,
                    uint16_t file_descr,
                    const char* path,
                    uint32_t size);

#endif //MINI_FS__HANDLERS_H_
