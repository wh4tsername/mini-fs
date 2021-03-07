#include <stdlib.h>
#include <string.h>

#include "client/client.h"
#include "fs/handlers/handlers.h"
#include <defines.h>

int main() {
    const int BUFFER_LENGTH = 256;
    char* buffer = malloc(BUFFER_LENGTH);

    while (read_command(buffer, BUFFER_LENGTH)) {
        char cmd[BUFFER_LENGTH];
        char* args = parse_token(buffer, cmd);

        if (strcmp(cmd, INIT_CMD) == 0) {
            init_fs();
        } else if (strcmp(cmd, DESTROY_CMD) == 0) {
            destroy_fs();
        } else if (strcmp(cmd, LS_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("ls command requires path arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            list_dir(path);
        } else if (strcmp(cmd, CREATE_DIR_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("mkdir command requires dir name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            create_dir(path);
        } else if (strcmp(cmd, DELETE_OBJ_CMD) == 0) {
            // no arg check
            if (args == NULL || strlen(args) == 0) {
                printf("rm command requires dir or file name arg!\n");
                continue;
            }

            char path[BUFFER_LENGTH];
            parse_token(args, path);

            delete_object(path);
        } else if (strlen(cmd) == 0) {
        } else {
            printf("unknown command!\n");
        }
    }

    free(buffer);

    return 0;
}
