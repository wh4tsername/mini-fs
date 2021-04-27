#ifndef MINI_FS__HANDLERS_H_
#define MINI_FS__HANDLERS_H_

#include <linux/types.h>

int init_fs(char* results, char* memory);

int destroy_fs(char* results, char* memory);

int list_dir(char* results, char* memory, const char* path);

int create_dir(char* results, char* memory, const char* path);

int delete_object(char* results, char* memory, const char* path);

int create_file(char* results, char* memory, const char* path);

int open_file(char* results, char* memory, const char* path);

int close_file(char* results, char* memory, __u16 file_descr);

int seek_pos(char* results, char* memory, __u16 file_descr,
              __u32 pos);

int write_to_file(char* results, char* memory, __u16 file_descr,
                   const char* path, __u32 size);

int read_from_file(char* results, char* memory, __u16 file_descr,
                    __u32 size);

#endif  // MINI_FS__HANDLERS_H_
