#ifndef MINI_FS_UTILS_HEADERS_IO_UTILS_H_
#define MINI_FS_UTILS_HEADERS_IO_UTILS_H_

int write_retry(int fd, const char* buffer, int buffer_size);
int read_retry(int fd, char* buffer, int buffer_size);

#endif  // MINI_FS_UTILS_HEADERS_IO_UTILS_H_
