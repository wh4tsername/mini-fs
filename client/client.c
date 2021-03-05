#include "client.h"
#include <stdint.h>

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

char* parse_cmd(char* buffer, char* cmd) {
    char* cmd_pos = strchr(buffer, ' ');
    if (cmd_pos == NULL) {
        strcpy(cmd, buffer);
    } else {
        uint16_t cmd_size = cmd_pos - buffer;

        memcpy(cmd, buffer, cmd_size);
        cmd[cmd_size] = '\0';
    }

    return cmd_pos;
}
