#include <stdlib.h>
#include <string.h>

#include "client/client.h"
#include "fs/handlers/handlers.h"
#include "utils/defines.h"

int main() {
    const int BUFFER_LENGTH = 256;
    char* buffer = malloc(BUFFER_LENGTH);

    while (read_command(buffer, BUFFER_LENGTH)) {
        char cmd[256];
        char* args = parse_cmd(buffer, cmd);

        if (strcmp(buffer, INIT_CMD) == 0) {
            init_fs();
        } else if (strcmp(buffer, DESTROY_CMD) == 0) {
            destroy_fs();
        } else if (strcmp(buffer, LS_CMD) == 0) {
            list_dir(); // TODO
        } else if (strcmp(buffer, CREATE_DIR_CMD) == 0) {
            create_dir(); // TODO
        } else if (strlen(buffer) == 0) {
        } else {
            printf("unknown command!\n");
        }
    }

    free(buffer);

    return 0;
}
