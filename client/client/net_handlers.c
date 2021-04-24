#include "net_handlers.h"

#include <constants/opcodes.h>
#include <net_utils.h>

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>

#include <defines.h>

void output_results(int sockd) {
  const int BUFFER_SIZE = 10000;
  char buffer[BUFFER_SIZE];

  long num_lines = 0;
  bool num_lines_is_set = false;
  long lines_counter = 0;
  while (!(num_lines_is_set && num_lines == lines_counter)) {
//    dprintf(STDERR_FILENO, "num_lines: %ld\n", num_lines);
//    dprintf(STDERR_FILENO, "counter: %ld\n", lines_counter);
//    dprintf(STDERR_FILENO, "is_set: %d\n", num_lines_is_set);

    ssize_t read = recv(sockd, buffer, BUFFER_SIZE, 0);
    conditional_handle_error(read == -1, "results receiving error");

    buffer[read] = '\0';

//    dprintf(STDERR_FILENO, "data: ");
//    write(STDERR_FILENO, buffer, read);
//    dprintf(STDERR_FILENO, "\n");

    char* buffer_start_pos = buffer;
    if (!num_lines_is_set && read >= 1) {
      num_lines = buffer[0] - '0';
      num_lines_is_set = true;
      ++buffer_start_pos;
      --read;
    }

    if (num_lines_is_set && read >= 1) {
      char* pointer = buffer_start_pos;
      while ((pointer = strchr(pointer, '\n')) != NULL) {
        ++lines_counter;
        ++pointer;
      }

      write(STDOUT_FILENO, buffer_start_pos, read);
    }

//    dprintf(STDERR_FILENO, "num_lines: %ld\n", num_lines);
//    dprintf(STDERR_FILENO, "counter: %ld\n", lines_counter);
//    dprintf(STDERR_FILENO, "is_set: %d\n", num_lines_is_set);
  }
}

void output_read_result(int sockd, uint32_t size, int dest_fd) {
  char* buffer = malloc(size);

  long long total = 0;
  while (total != size) {
    ssize_t read = recv(sockd, buffer, size, 0);

    conditional_handle_error(read == -1, "read results receiving error");

    write(dest_fd, buffer, read);

    total += read;
  }

  free(buffer);
}

void send_init_fs(int sockd) {
  send_opcode(sockd, FS_INIT);
}

void send_destroy_fs(int sockd) {
  send_opcode(sockd, FS_DESTROY);
}

void send_list_dir(int sockd, const char* path) {
  send_opcode(sockd, FS_LS);

  send_string(sockd, path);

  output_results(sockd);
}

void send_create_dir(int sockd, const char* path) {
  send_opcode(sockd, FS_MKDIR);

  send_string(sockd, path);
}

void send_delete_object(int sockd, const char* path) {
  send_opcode(sockd, FS_RM);

  send_string(sockd, path);
}

void send_create_file(int sockd, const char* path) {
  send_opcode(sockd, FS_TOUCH);

  send_string(sockd, path);
}

void send_open_file(int sockd, const char* path) {
  send_opcode(sockd, FS_OPEN);

  send_string(sockd, path);

  output_results(sockd);
}

void send_close_file(int sockd, uint16_t file_descr) {
  send_opcode(sockd, FS_CLOSE);

  send_uint16_t(sockd, file_descr);
}

void send_seek_pos(int sockd, uint16_t file_descr, uint32_t pos) {
  send_opcode(sockd, FS_SEEK);

  send_uint16_t(sockd, file_descr);
  send_uint32_t(sockd, pos);
}

void send_write_to_file(int sockd, uint16_t file_descr,
                        const char* path, uint32_t size) {
  send_opcode(sockd, FS_WRITE);

  send_uint16_t(sockd, file_descr);

  char* content = malloc(size);
  int source_fd = open(path, O_RDONLY, S_IRUSR | S_IWUSR);
  conditional_handle_error(read(source_fd, content, size) == -1,
                           "error while reading from src file for write operation");

  send_string(sockd, content);

  close(source_fd);
  free(content);

  send_uint32_t(sockd, size);
}

void send_read_from_file(int sockd, uint16_t file_descr,
                         const char* path, uint32_t size) {
  send_opcode(sockd, FS_READ);

  send_uint16_t(sockd, file_descr);

  int dest_fd = open(path, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);

  send_uint32_t(sockd, size);

  output_read_result(sockd, size, dest_fd);

  close(dest_fd);
}

void send_quit(int sockd) {
  send_opcode(sockd, FS_QUIT);
}
