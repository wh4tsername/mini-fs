#include "handlers.h"

#include <defines.h>
#include <constants/fs_constants.h>
#include <helpers.h>
#include <utils.h>

void close_file(const char* descr_str) {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    // get descriptor table
    struct descriptor_table dt;
    reset_descriptor_table(&dt);

    read_from_descriptor_table(fd, &dt);

    uint16_t file_descr = strtol(descr_str, NULL, 10);
    if (errno == ERANGE) {
        panic("incorrect file descriptor");
    }

    // free file descriptor
    free_descriptor(&dt, file_descr);

    // write descriptor table
    write_to_descriptor_table(fd, &dt);

    close(fd);
}
