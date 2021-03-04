#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "handlers.h"
#include "utils.h"

int main() {
    const int BUFFER_LENGTH = 256;
    char* buffer = malloc(BUFFER_LENGTH);

    while (read(buffer, BUFFER_LENGTH)) {
        if (strcmp(buffer, INIT_CMD) == 0) {
            init_fs();
        } else {
            PANIC("unknown command!");
        }
    }

    return 0;
}
