#include "handlers.h"
#include "../../utils/defines.h"
#include "../../utils/constants/fs_constants.h"

void create_dir() {
    int fd = open(FS_FILENAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    dprintf(fd, "dir\n");

    close(fd);
}
