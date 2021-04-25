#include <string.h>
#include <string_utils.h>

int32_t find_last_occurrence(const char* str, char ch) {
  uint16_t length = strlen(str);
  for (int i = length - 1; i >= 0; --i) {
    if (*(str + i) == ch) {
      return i;
    }
  }

  return -1;
}

void delete_last_slash_and_copy_res(const char* str, uint16_t str_length,
                                    char* dest) {
  memcpy(dest, str, str_length);
  if (*(str + str_length - 1) != '/') {
    dest[str_length] = '\0';
  } else {
    dest[str_length - 1] = '\0';
  }
}
