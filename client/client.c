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
