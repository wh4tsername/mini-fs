#include "client.h"

bool read(char* buffer, int buffer_size) {
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
