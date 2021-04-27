#ifndef MINI_FS_UTILS_HEADERS_IO_UTILS_H_
#define MINI_FS_UTILS_HEADERS_IO_UTILS_H_

#include <linux/types.h>

void write_retry(char* memory, const char* buffer, int buffer_size, __u32 offset);
void read_retry(const char* memory, char* buffer, int buffer_size, __u32 offset);

#endif  // MINI_FS_UTILS_HEADERS_IO_UTILS_H_
