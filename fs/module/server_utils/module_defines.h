#ifndef MINI_FS_FS_MODULE_MODULE_DEFINES_H_
#define MINI_FS_FS_MODULE_MODULE_DEFINES_H_

#include <stdio.h>
#include <stdlib.h>

#define cond_server_panic(stmt, msg) \
  if (stmt) {                        \
    fprintf(stderr, "SERVER LOG: "); \
    fprintf(stderr, "%s\n", msg);    \
    exit(EXIT_FAILURE);              \
  }

#define server_panic(msg)          \
  fprintf(stderr, "SERVER LOG: "); \
  fprintf(stderr, "%s\n", msg);    \
  exit(EXIT_FAILURE);

#endif  // MINI_FS_FS_MODULE_MODULE_DEFINES_H_
