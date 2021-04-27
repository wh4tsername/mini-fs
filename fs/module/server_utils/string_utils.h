#ifndef MINI_FS_UTILS_HEADERS_STRING_UTILS_H_
#define MINI_FS_UTILS_HEADERS_STRING_UTILS_H_

#include <linux/types.h>

int find_last_occurrence(const char* str, char ch, __s32* pos);

int delete_last_slash_and_copy_res(const char* str, __u16 str_length,
                                    char* dest);

#endif  // MINI_FS_UTILS_HEADERS_STRING_UTILS_H_
