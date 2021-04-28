#include "client.h"

#include <commands.h>

#include "../../fs/module/constants/constants.h"

#include <defines.h>
#include <stdbool.h>
#include <string.h>

#include "connection.h"
#include "net_handlers.h"

bool read_command(char* buffer, int buffer_size) {
  printf("mini-fs$ ");

  fgets(buffer, buffer_size, stdin);

  unsigned int cmd_length = strlen(buffer);

  // cut '\n' symbol
  buffer[cmd_length - 1] = '\0';

  if (strcmp(buffer, QUIT_CMD) == 0) {
    return false;
  }

  return true;
}

char* parse_token(char* buffer, char* token) {
  char* token_pos = strchr(buffer, ' ');
  if (token_pos == NULL) {
    strcpy(token, buffer);
  } else {
    uint16_t cmd_size = token_pos - buffer;

    memcpy(token, buffer, cmd_size);
    token[cmd_size] = '\0';
  }

  return token_pos == NULL ? NULL : token_pos + 1;
}

int client(const char* address_string) {
  int sockd = establish_connection(address_string);

  char buffer[MAX_PATH_LENGTH];

  while (read_command(buffer, MAX_PATH_LENGTH)) {
    char cmd[MAX_PATH_LENGTH];
    char* args = parse_token(buffer, cmd);

    if (strcmp(cmd, INIT_CMD) == 0) {
      send_init_fs(sockd);
    } else if (strcmp(cmd, DESTROY_CMD) == 0) {
      send_destroy_fs(sockd);
    } else if (strcmp(cmd, LS_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("ls command requires path arg!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      parse_token(args, path);

      send_list_dir(sockd, path);
    } else if (strcmp(cmd, CREATE_DIR_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("mkdir command requires dir name arg!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      parse_token(args, path);

      send_create_dir(sockd, path);
    } else if (strcmp(cmd, DELETE_OBJ_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("rm command requires dir or file name arg!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      parse_token(args, path);

      send_delete_object(sockd, path);
    } else if (strcmp(cmd, CREATE_FILE_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("touch command requires file name arg!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      parse_token(args, path);

      send_create_file(sockd, path);
    } else if (strcmp(cmd, OPEN_FILE_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("open command requires file name arg!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      parse_token(args, path);

      send_open_file(sockd, path);
    } else if (strcmp(cmd, CLOSE_FILE_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("close command requires file descriptor arg!\n");
        continue;
      }

      char descr_str[MAX_PATH_LENGTH];
      parse_token(args, descr_str);

      uint16_t file_descr = strtol(descr_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      send_close_file(sockd, file_descr);
    } else if (strcmp(cmd, SEEK_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf("seek command requires file descriptor and pos args!\n");
        continue;
      }

      char descr_str[MAX_PATH_LENGTH];
      args = parse_token(args, descr_str);

      uint16_t file_descr = strtol(descr_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      // one arg check
      if (args == NULL || strlen(args) == 0) {
        printf("seek command requires file descriptor and pos args!\n");
        continue;
      }

      char pos_str[MAX_PATH_LENGTH];
      parse_token(args, pos_str);

      uint32_t pos = strtol(pos_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect position in file");
      }

      send_seek_pos(sockd, file_descr, pos);
    } else if (strcmp(cmd, WRITE_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "write command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char descr_str[MAX_PATH_LENGTH];
      args = parse_token(args, descr_str);

      uint16_t file_descr = strtol(descr_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      // one arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "write command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      args = parse_token(args, path);

      // two arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "write command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char size_str[MAX_PATH_LENGTH];
      parse_token(args, size_str);

      uint32_t size = strtol(size_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      send_write_to_file(sockd, file_descr, path, size);
    } else if (strcmp(cmd, READ_CMD) == 0) {
      // no arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "read command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char descr_str[MAX_PATH_LENGTH];
      args = parse_token(args, descr_str);

      uint16_t file_descr = strtol(descr_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      // one arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "read command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char path[MAX_PATH_LENGTH];
      args = parse_token(args, path);

      // two arg check
      if (args == NULL || strlen(args) == 0) {
        printf(
            "read command requires file descriptor, "
            "source file path and size args!\n");
        continue;
      }

      char size_str[MAX_PATH_LENGTH];
      parse_token(args, size_str);

      uint32_t size = strtol(size_str, NULL, 10);
      if (errno == ERANGE) {
        panic("incorrect file descriptor");
      }

      send_read_from_file(sockd, file_descr, path, size);
    } else if (strlen(cmd) == 0) {
    } else {
      printf("unknown command!\n");
    }
  }

  send_quit(sockd);
  close_connection(sockd);

  return 0;
}
