#include "handlers.h"
#include "../../utils/constants/fs_constants.h"
#include "../../utils/defines.h"

void destroy_fs() {
    int fd = open(FS_FILENAME, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    ftruncate(fd, FS_SIZE);

    close(fd);
}
