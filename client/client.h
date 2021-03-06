#ifndef MINI_FS__CLIENT_H_
#define MINI_FS__CLIENT_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <constants/commands.h>

// read function
bool read_command(char* buffer, int buffer_size);

// parse cmd
char* parse_token(char* buffer, char* token);

#endif //MINI_FS__CLIENT_H_
