#include "fs_io_utils.h"

#include <linux/string.h>

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

void write_retry(char* memory, const char* buffer, int buffer_size, __u32 offset) {
  memcpy(memory + offset, buffer, buffer_size);
}

void read_retry(const char* memory, char* buffer, int buffer_size, __u32 offset) {
  memcpy(buffer, memory + offset, buffer_size);
}
