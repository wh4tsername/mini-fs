#include <linux/string.h>
#include "string_utils.h"

int find_last_occurrence(const char* str, char ch, __s32* pos) {
  __u16 length = strlen(str);
  for (int i = length - 1; i >= 0; --i) {
    if (*(str + i) == ch) {
      *pos = i;
      return 0;
    }
  }

  return -1;
}

int delete_last_slash_and_copy_res(const char* str, __u16 str_length,
                                    char* dest) {
  memcpy(dest, str, str_length);
  if (*(str + str_length - 1) != '/') {
    dest[str_length] = '\0';
  } else {
    dest[str_length - 1] = '\0';
  }

  return 0;
}
