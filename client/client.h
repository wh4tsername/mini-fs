#ifndef MINI_FS__CLIENT_H_
#define MINI_FS__CLIENT_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../utils/constants/commands.h"

// read function
bool read_command(char* buffer, int buffer_size);

// parse cmd
char* parse_cmd(char* buffer, char* cmd);

#endif //MINI_FS__CLIENT_H_
