#include "handlers.h"
#include <constants/fs_constants.h>
#include <defines.h>

void destroy_fs() {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    close(fd);
}
