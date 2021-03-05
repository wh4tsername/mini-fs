#include "utils.h"
#include "constants/constants.h"
#include "constants/struct_sizes.h"
#include "defines.h"

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

    conditional_handle_error(block_id > NUM_BLOCKS, "incorrect block_id");
    conditional_handle_error(inblock_offset >= BLOCK_SIZE, "incorrect inblock offset");

    lseek(fd, offset, SEEK_SET);
    conditional_handle_error(write_retry(fd, buffer, buffer_size) == -1,
                             "error while writing to file");
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
