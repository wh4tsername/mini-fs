#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <disk_utils.h>

void close_file(const char* fs_path, uint16_t file_descr) {
    int fd = open(fs_path, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    read_descriptor_table(fd, &dt);

    // free file descriptor
    free_descriptor(&dt, file_descr);

    // write descriptor table
    write_descriptor_table(fd, &dt);

    close(fd);
}
