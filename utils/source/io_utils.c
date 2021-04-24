#include "io_utils.h"

#include <unistd.h>

int write_retry(const int fd, const char* buffer,
                const int buffer_size) {
  int total_bytes = 0;
  int bytes_written = 0;

  while (total_bytes != buffer_size) {
    bytes_written = (int)write(fd, &buffer[total_bytes],
                          buffer_size - total_bytes);

    if (bytes_written == -1) {
      return bytes_written;
    }

    total_bytes += bytes_written;
  }

  return total_bytes;
}

int read_retry(const int fd, char* buffer,
               const int buffer_size) {
  int total_bytes = 0;
  int bytes_read = 0;

  while (total_bytes != buffer_size) {
    bytes_read = (int)read(fd, &buffer[total_bytes],
                      buffer_size - total_bytes);

    if (bytes_read == -1) {
      return bytes_read;
    }

    total_bytes += bytes_read;
  }

  return total_bytes;
}
