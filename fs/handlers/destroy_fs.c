#include "handlers.h"
#include <constants/fs_constants.h>
#include <defines.h>

void destroy_fs(const char* fs_path) {
    int fd = open(fs_path, O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    close(fd);
}
