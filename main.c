#include <stdlib.h>

#include "client.h"

int main() {
    const int BUFFER_LENGTH = 256;
    char* buffer = malloc(BUFFER_LENGTH);

    while (read(buffer, BUFFER_LENGTH)) {

    }

    return 0;
}
