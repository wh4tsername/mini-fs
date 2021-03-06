#include "handlers.h"
#include "../../utils/defines.h"
#include "../../utils/utils.h"
#include "../../utils/constants/fs_constants.h"

const char* find_last_occurrence(const char* str, char ch) {
    uint16_t length = strlen(str);
    for (int i = length - 1; i >= 0; --i) {
        if (*(str + i) == ch) {
            return str + i;
        }
    }

    return NULL;
}

void delete_last_slash_and_copy_res(const char* str,
                                    uint16_t str_length,
                                    char* dest) {
    memcpy(dest, str, str_length);
    if (*(str + str_length - 1) != '/') {
        dest[str_length] = '\0';
    } else {
        dest[str_length - 1] = '\0';
    }
}

void create_dir(const char* path) {
    int fd = open(FS_FILENAME, O_RDWR, S_IRUSR | S_IWUSR);
    conditional_parse_errno(fd == -1);

    uint16_t path_length = strlen(path);
    char buffer[256];
    delete_last_slash_and_copy_res(path, path_length, buffer);

    conditional_handle_error(path[0] != '/' || strlen(buffer) == 0,
                             "incorrect path");

    const char* dir_name = find_last_occurrence(buffer, '/') + 1;

    char path_to_traverse[256];
    memcpy(path_to_traverse, buffer, dir_name - buffer);
    path_to_traverse[dir_name - buffer] = '\0';

    printf("%s\n", path_to_traverse);
    printf("%s\n", dir_name);

    // get superblock
    struct superblock sb;
    init_superblock(&sb);
    read_from_superblock(fd, &sb);

    // get inode of prev dir
    struct inode inode;
    init_inode(&inode);
    traverse_path(fd, path_to_traverse, &inode);

    // get block of prev dir
    struct dir_record record;
    init_dir_record(&record);
    read_from_block(fd,
                    inode.block_ids[0],
                    0,
                    (char*)&record,
                    DIR_RECORD_SIZE);
    uint16_t inode_id = record.inode_id;

    uint16_t next_inode_id = create_dir_block_and_inode(fd, &sb, false, inode_id);

    // create dir_record for prev dir
    init_dir_record(&record);
    record.inode_id = next_inode_id;
    strcpy(record.name, dir_name);

    // inode of prev dir info update
    conditional_handle_error(inode.size >= 16, "directory overflow");
    inode.size += 1;
    write_to_inode(fd,
                   inode_id,
                   &inode);

    // make new dir_record
    write_to_block(fd,
                   inode.block_ids[0],
                   (inode.size - 1) * DIR_RECORD_SIZE,
                   (const char *)&record,
                   DIR_RECORD_SIZE);

    // write superblock
    write_to_superblock(fd, &sb);

    close(fd);
}
