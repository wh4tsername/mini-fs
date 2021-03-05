#include "dir_record.h"

void init_dir_record(struct dir_record* dir_record_pointer) {
    dir_record_pointer->inode_id = 0;

    for (int i = 0; i < DIR_NAME_LENGTH; ++i) {
        dir_record_pointer->name[i] = '\0';
    }
}
