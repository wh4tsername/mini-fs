#ifndef MINI_FS__CLIENT_H_
#define MINI_FS__CLIENT_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "commands.h"

// read function
bool read(char* buffer, int buffer_size);

#endif //MINI_FS__CLIENT_H_
