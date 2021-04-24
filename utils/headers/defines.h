#ifndef MINI_FS_UTILS_DEFINE_H_
#define MINI_FS_UTILS_DEFINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define panic(msg)                \
    fprintf(stderr, "%s\n", msg); \
    exit(EXIT_FAILURE);

#define conditional_handle_error(stmt, msg) \
    if (stmt) {                             \
      fprintf(stderr, "%s\n", msg);         \
      exit(EXIT_FAILURE);                   \
    }

#define conditional_parse_errno(stmt)           \
    if (stmt) {                                 \
      fprintf(stderr, "%s\n", strerror(errno)); \
      exit(EXIT_FAILURE);                       \
    }

#endif //MINI_FS_UTILS_DEFINE_H_
