#include "utils.h"
#include "constants/constants.h"
#include "constants/struct_sizes.h"
#include "defines.h"

#include <string.h>

int write_retry(const int fd, const char* buffer,
                const int buffer_size) {
    int total_bytes = 0;
    int bytes_written = 0;

    while (total_bytes != buffer_size) {
        bytes_written = write(fd, &buffer[total_bytes],
                              buffer_size - total_bytes);

        if (bytes_written == -1) {
            return bytes_written;
        }

        total_bytes += bytes_written;
    }

    return total_bytes;
}

int read_retry(const int fd, char* buffer,
               const int buffer_size) {
    int total_bytes = 0;
    int bytes_read = 0;

    while (total_bytes != buffer_size) {
        bytes_read = read(fd, &buffer[total_bytes],
                          buffer_size - total_bytes);

        if (bytes_read == -1) {
            return bytes_read;
        }

        total_bytes += bytes_read;
    }

    return total_bytes;
}

void write_to_inode(int fd,
                    uint16_t inode_id,
                    const struct inode* inode) {
    uint32_t offset =
        SUPERBLOCK_SIZE +
        DESCRIPTOR_TABLE_SIZE +
        INODE_SIZE * (inode_id - 1);

    conditional_handle_error(inode_id > NUM_INODES,
                             "incorrect inode_id");

    lseek(fd, offset, SEEK_SET);
    conditional_handle_error(write_retry(fd, (const char*)inode, INODE_SIZE) == -1,
                             "error while writing to file");
}

void read_from_inode(int fd,
                     uint16_t inode_id,
                     struct inode* inode) {
    uint32_t offset =
        SUPERBLOCK_SIZE +
        DESCRIPTOR_TABLE_SIZE +
        INODE_SIZE * (inode_id - 1);

    conditional_handle_error(inode_id > NUM_INODES,
                             "incorrect inode_id");

    lseek(fd, offset, SEEK_SET);
    conditional_handle_error(read_retry(fd, (char*)inode, INODE_SIZE) == -1,
                             "error while reading from file");
}

void write_to_block(const int fd,
                    uint16_t block_id,
                    uint16_t inblock_offset,
                    const char* buffer,
                    const int buffer_size) {
    uint32_t offset =
        SUPERBLOCK_SIZE +
        DESCRIPTOR_TABLE_SIZE +
        INODE_SIZE * NUM_INODES +
        BLOCK_SIZE * (block_id - 1) +
        inblock_offset;

    conditional_handle_error(block_id > NUM_BLOCKS,
                             "incorrect block_id");
    conditional_handle_error(inblock_offset + buffer_size >= BLOCK_SIZE,
                             "write to block out of bounds");

    lseek(fd, offset, SEEK_SET);
    conditional_handle_error(write_retry(fd, buffer, buffer_size) == -1,
                             "error while writing to file");
}

void read_from_block(int fd,
                     uint16_t block_id,
                     uint16_t inblock_offset,
                     char* buffer,
                     int buffer_size) {
    uint32_t offset =
        SUPERBLOCK_SIZE +
        DESCRIPTOR_TABLE_SIZE +
        INODE_SIZE * NUM_INODES +
        BLOCK_SIZE * (block_id - 1) +
        inblock_offset;

    conditional_handle_error(block_id > NUM_BLOCKS,
                             "incorrect block_id");
    conditional_handle_error(inblock_offset + buffer_size >= BLOCK_SIZE,
                             "read from block out of bounds");

    lseek(fd, offset, SEEK_SET);
    conditional_handle_error(read_retry(fd, buffer, buffer_size) == -1,
                             "error while reading from file");
}

void write_dir_records(int fd,
                       uint16_t block_id,
                       struct dir_record* records,
                       uint16_t num_records) {
    for (uint16_t i = 0; i < num_records; ++i) {
        write_to_block(fd,
                       block_id,
                       i * DIR_RECORD_SIZE,
                       (const char*)&records[i],
                       DIR_RECORD_SIZE);
    }
}

void read_dir_records(int fd,
                      uint16_t block_id,
                      struct dir_record* records,
                      uint16_t num_records) {
    for (uint16_t i = 0; i < num_records; ++i) {
        read_from_block(fd,
                        block_id,
                        i * DIR_RECORD_SIZE,
                        (char *)&records[i],
                        DIR_RECORD_SIZE);
    }
}

// traverse recursive
void traverse_recursively(int fd,
                          const char* path,
                          struct inode* current,
                          struct inode* res) {
    conditional_handle_error(current->is_file,
                             "trying to enter file");

    char current_layer[256];
    char* remaining_path = parse_path(path, current_layer);

    if (strcmp(current_layer, "/") == 0) {
        memcpy(res, current, sizeof(struct inode));
    } else {
        struct dir_record records[15];

        read_dir_records(fd, current->block_ids[0], records, current->size);

        bool dir_found = false;
        uint16_t i = 0;
        for (; i < current->size; ++i) {
            if (strcmp(records[i].name, current_layer) == 0) {
                dir_found = true;

                break;
            }
        }

        conditional_handle_error(!dir_found, "directory doesn't exist");

        struct inode next;
        read_from_inode(fd, records[i].inode_id, &next);

        if (remaining_path == NULL) {
            memcpy(res, &next, sizeof(struct inode));
        } else {
            traverse_recursively(fd, remaining_path, &next, res);
        }
    }
}

void traverse_path(int fd, const char* path, struct inode* res) {
    // inode of "/" = 1
    struct inode cur_inode;
    read_from_inode(fd, 1, &cur_inode);

    traverse_recursively(fd, path, &cur_inode, res);
}

char* parse_path(const char* path, char* next_token) {
    char* slash_pos = strchr(path, '/');
    conditional_handle_error(slash_pos == NULL, "incorrect path");

    uint16_t path_length = strlen(path);
    printf("path length: %hu\n", path_length);

    // path = "/"
    if (path_length == 1) {
        strcpy(next_token, "/");
        return NULL;
    }

    char* next_slash_pos = strchr(slash_pos + 1, '/');
    uint16_t next_token_length = 0;
    if (next_slash_pos == NULL) {
        next_token_length = path + path_length - slash_pos - 1;
    } else {
        next_token_length = next_slash_pos - slash_pos - 1;

        conditional_handle_error(next_token_length == 0, "incorrect path");
    }

    printf("next_token_length: %hu\n", next_token_length);

    memcpy(next_token, slash_pos + 1, next_token_length);
    next_token[next_token_length] = '\0';

    return next_slash_pos;
}
