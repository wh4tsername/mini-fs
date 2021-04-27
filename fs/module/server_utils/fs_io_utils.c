#include "fs_io_utils.h"

#include <linux/string.h>

void write_retry(char* memory, const char* buffer, int buffer_size, __u32 offset) {
  memcpy(memory + offset, buffer, buffer_size);
}

void read_retry(const char* memory, char* buffer, int buffer_size, __u32 offset) {
  memcpy(buffer, memory + offset, buffer_size);
}
