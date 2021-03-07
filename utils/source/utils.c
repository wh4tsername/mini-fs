#include <utils.h>
#include <constants/constants.h>
#include <constants/struct_sizes.h>
#include <defines.h>

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

void write_to_superblock(int fd, const struct superblock* sb) {
    lseek(fd, 0, SEEK_SET);
    conditional_handle_error(write_retry(fd, (const char*)sb, SUPERBLOCK_SIZE) == -1,
                             "error while writing to file");
}

void read_from_superblock(int fd, struct superblock* sb) {
    lseek(fd, 0, SEEK_SET);
    conditional_handle_error(read_retry(fd, (char*)sb, SUPERBLOCK_SIZE) == -1,
                             "error while reading from file");
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
    conditional_handle_error(inblock_offset + buffer_size > BLOCK_SIZE,
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

void make_zeros(int fd, uint32_t offset, uint32_t size) {
    char* buffer = malloc(size);
    memset(buffer, '\0', size);

    lseek(fd, offset, SEEK_SET);
    write_retry(fd, buffer, size);

    free(buffer);
}

void erase_block(int fd, uint16_t block_id) {
    uint32_t size = BLOCK_SIZE;

    char* buffer = malloc(size);
    memset(buffer, '\0', size);

    write_to_block(fd, block_id, 0, buffer, size);

    free(buffer);
}

void erase_inode(int fd, uint16_t inode_id) {
    struct inode inode;
    reset_inode(&inode);

    write_to_inode(fd, inode_id, &inode);
}
