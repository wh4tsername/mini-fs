#include <helpers.h>
#include <utils.h>
#include <defines.h>
#include "../../fs/dir_record.h"

#include <string.h>

uint16_t create_dir_block_and_inode(int fd,
                                    struct superblock* sb,
                                    bool is_root,
                                    uint16_t prev_inode_id) {
    uint16_t block_id = occupy_block(sb);
    uint16_t inode_id = occupy_inode(sb);

    // create inode
    struct inode inode;
    init_inode(&inode);
    inode.block_ids[0] = block_id;
    inode.is_file = false;
    inode.size = 2;

    // create dir_records
    struct dir_record records[2];
    init_dir_record(&records[0]);
    init_dir_record(&records[1]);
    records[0].inode_id = inode_id;
    records[1].inode_id = is_root ? inode_id : prev_inode_id;
    strcpy(records[0].name,".");
    strcpy(records[1].name,"..");

    write_dir_records(fd, block_id, records, inode.size);
    write_to_inode(fd, inode_id, &inode);

    return inode_id;
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
        struct dir_record records[16];

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
    // TODO should check res->is_file?
}

char* parse_path(const char* path, char* next_token) {
    char* slash_pos = strchr(path, '/');
    conditional_handle_error(slash_pos == NULL, "incorrect path");

    uint16_t path_length = strlen(path);
//    printf("path length: %hu\n", path_length);

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

//    printf("next_token_length: %hu\n", next_token_length);

    memcpy(next_token, slash_pos + 1, next_token_length);
    next_token[next_token_length] = '\0';

    return next_slash_pos;
}
