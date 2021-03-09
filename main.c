#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "client/client.h"
#include "fs/handlers/handlers.h"
#include <defines.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        panic("file system storage file should be specified as an argument");
    }

    const char* fs_path = argv[1];

    const int BUFFER_LENGTH = 256;
    char buffer[BUFFER_LENGTH];

    while (read_command(buffer, BUFFER_LENGTH)) {
        char cmd[BUFFER_LENGTH];
        char* args = parse_token(buffer, cmd);

        if (strcmp(cmd, INIT_CMD) == 0) {
            init_fs(fs_path);
        } else if (strcmp(cmd, DESTROY_CMD) == 0) {
            destroy_fs(fs_path);
        } else if (strcmp(cmd, LS_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("ls command requires path arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            list_dir(fs_path, path);
        } else if (strcmp(cmd, CREATE_DIR_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("mkdir command requires dir name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            create_dir(fs_path, path);
        } else if (strcmp(cmd, DELETE_OBJ_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("rm command requires dir or file name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            delete_object(fs_path, path);
        } else if (strcmp(cmd, CREATE_FILE_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("touch command requires file name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            create_file(fs_path, path);
        } else if (strcmp(cmd, OPEN_FILE_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("open command requires file name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            open_file(fs_path, path);
        } else if (strcmp(cmd, CLOSE_FILE_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("close command requires file descriptor arg!\n");
                continue;
            }

            char descr_str[BUFFER_LENGTH];
            parse_token(args, descr_str);

            uint16_t file_descr = strtol(descr_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect file descriptor");
            }

            close_file(fs_path, file_descr);
        } else if (strcmp(cmd, SEEK_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("seek command requires file descriptor and pos args!\n");
                continue;
            }

            char descr_str[BUFFER_LENGTH];
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

            char pos_str[BUFFER_LENGTH];
            parse_token(args, pos_str);

            uint32_t pos = strtol(pos_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect position in file");
            }

            seek_pos(fs_path, file_descr, pos);
        } else if (strcmp(cmd, WRITE_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("write command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char descr_str[BUFFER_LENGTH];
            args = parse_token(args, descr_str);

            uint16_t file_descr = strtol(descr_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect file descriptor");
            }

            // one arg check
            if (args == NULL || strlen(args) == 0) {
                printf("write command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            args = parse_token(args, path);

            // two arg check
            if (args == NULL || strlen(args) == 0) {
                printf("write command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char size_str[BUFFER_LENGTH];
            parse_token(args, size_str);

            uint16_t size = strtol(size_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect file descriptor");
            }

            write_to_file(fs_path, file_descr, path, size);
        } else if (strcmp(cmd, READ_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("read command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char descr_str[BUFFER_LENGTH];
            args = parse_token(args, descr_str);

            uint16_t file_descr = strtol(descr_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect file descriptor");
            }

            // one arg check
            if (args == NULL || strlen(args) == 0) {
                printf("read command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            args = parse_token(args, path);

            // two arg check
            if (args == NULL || strlen(args) == 0) {
                printf("read command requires file descriptor, "
                       "source file path and size args!\n");
                continue;
            }

            char size_str[BUFFER_LENGTH];
            parse_token(args, size_str);

            uint16_t size = strtol(size_str, NULL, 10);
            if (errno == ERANGE) {
                panic("incorrect file descriptor");
            }

            read_from_file(fs_path, file_descr, path, size);
        } else if (strlen(cmd) == 0) {
        } else {
            printf("unknown command!\n");
        }
    }

    return 0;
}
