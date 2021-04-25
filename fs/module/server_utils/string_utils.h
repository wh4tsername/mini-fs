#ifndef MINI_FS_UTILS_HEADERS_STRING_UTILS_H_
#define MINI_FS_UTILS_HEADERS_STRING_UTILS_H_

#include <stdint.h>

int32_t find_last_occurrence(const char* str, char ch);

void delete_last_slash_and_copy_res(const char* str, uint16_t str_length,
                                    char* dest);

#endif  // MINI_FS_UTILS_HEADERS_STRING_UTILS_H_
