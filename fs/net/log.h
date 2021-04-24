#ifndef MINI_FS_FS_NET_LOG_H_
#define MINI_FS_FS_NET_LOG_H_

#include <stdio.h>

#define log(format, arg)                                   \
  dprintf(STDERR_FILENO, "SERVER LOG: ");                  \
  dprintf(STDERR_FILENO, format, arg);                     \
  dprintf(STDERR_FILENO, "\n");

#endif //MINI_FS_FS_NET_LOG_H_
