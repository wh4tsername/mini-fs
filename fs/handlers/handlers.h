#ifndef MINI_FS__HANDLERS_H_
#define MINI_FS__HANDLERS_H_

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void init_fs();

void destroy_fs();

void list_dir(const char* path);

void create_dir(const char* path);

void delete_object(const char* path);

void create_file(const char* path);

#endif //MINI_FS__HANDLERS_H_
