#include "client.h"

#include <stdio.h>
#include <string.h>

const char* QUIT_CMD_ = "quit";

bool read(char* buffer, int buffer_size) {
    printf("mini-fs$ ");

    fgets(buffer, buffer_size, stdin);

    unsigned int cmd_length = strlen(buffer);

    // cut '\n' symbol
    buffer[cmd_length - 1] = '\0';

    if (strcmp(buffer, QUIT_CMD_) == 0) {
        return false;
    }

    return true;
}
