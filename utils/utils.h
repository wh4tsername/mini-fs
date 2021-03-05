#ifndef MINI_FS__UTILS_H_
#define MINI_FS__UTILS_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "../fs/dir_record.h"

int write_retry(int fd, const char* buffer, int buffer_size);

int read_retry(int fd, char* buffer, int buffer_size);

void write_to_block(int fd,
                    uint16_t block_id,
                    uint16_t inblock_offset,
                    const char* buffer,
                    int buffer_size);

void write_dir_records(int fd,
                       uint16_t block_id,
                       struct dir_record* records,
                       uint16_t num_records);

#endif //MINI_FS__UTILS_H_
