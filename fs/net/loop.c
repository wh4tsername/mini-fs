#include "loop.h"

#include <constants/opcodes.h>
#include <defines.h>
#include <net_utils.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../module/handlers/handlers.h"
#include "log.h"

#define FS_FILE "../../fs.bin"

bool decode_commands(int fd) {
  enum OPCODE op;
  recv_opcode(fd, &op);

  char* path = NULL;
  char* data = NULL;
  bool ret = true;
  uint16_t file_descr = 0;
  uint32_t pos = 0;
  switch (op) {
    case FS_QUIT:
      log("command: quit", NULL);

      ret = false;
      break;

    case FS_INIT:
      log("command: init", NULL);

      init_fs(fd, FS_FILE);
      break;

    case FS_DESTROY:
      log("command: destroy", NULL);

      destroy_fs(fd, FS_FILE);
      break;

    case FS_LS:
      log("command: ls", NULL);

      recv_string(fd, &path);

      list_dir(fd, FS_FILE, path);
      break;

    case FS_MKDIR:
      log("command: mkdir", NULL);

      recv_string(fd, &path);

      create_dir(fd, FS_FILE, path);
      break;

    case FS_RM:
      log("command: rm", NULL);

      recv_string(fd, &path);

      delete_object(fd, FS_FILE, path);
      break;

    case FS_TOUCH:
      log("command: touch", NULL);

      recv_string(fd, &path);

      create_file(fd, FS_FILE, path);
      break;

    case FS_OPEN:
      log("command: open", NULL);

      recv_string(fd, &path);

      open_file(fd, FS_FILE, path);
      break;

    case FS_CLOSE:
      log("command: close", NULL);

      recv_uint16_t(fd, &file_descr);

      close_file(fd, FS_FILE, file_descr);
      break;

    case FS_SEEK:
      log("command: seek", NULL);

      recv_uint16_t(fd, &file_descr);
      recv_uint32_t(fd, &pos);

      seek_pos(fd, FS_FILE, file_descr, pos);
      break;

    case FS_WRITE:
      log("command: write", NULL);

      recv_uint16_t(fd, &file_descr);
      recv_string(fd, &data);
      recv_uint32_t(fd, &pos);

      log("file_descr: %d", file_descr);
      log("size: %d", pos);

      write_to_file(fd, FS_FILE, file_descr, data, pos);
      break;

    case FS_READ:
      log("command: read", NULL);

      recv_uint16_t(fd, &file_descr);
      recv_uint32_t(fd, &pos);

      log("file_descr: %d", file_descr);
      log("size: %d", pos);

      read_from_file(fd, FS_FILE, file_descr, pos);
      break;
  }

  free(data);
  free(path);

  return ret;
}

int server_loop(long port, int stop_fd) {
  int connection_fd = socket(AF_INET, SOCK_STREAM, 0);
  conditional_handle_error(connection_fd == -1, "socket error");

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  conditional_parse_errno(
      bind(connection_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1);

  conditional_handle_error(listen(connection_fd, 1) == -1, "listen error");

  int epoll_fd = epoll_create1(0);
  {
    int fds[] = {stop_fd, connection_fd, -1};
    for (int* fd = fds; *fd != -1; ++fd) {
      struct epoll_event event;
      memset(&event, 0, sizeof(struct epoll_event));
      event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
      event.data.fd = *fd;

      epoll_ctl(epoll_fd, EPOLL_CTL_ADD, *fd, &event);
    }
  }

  while (true) {
    struct epoll_event event;
    memset(&event, 0, sizeof(struct epoll_event));
    int epoll_ret = epoll_wait(epoll_fd, &event, 1, 1000);
    if (epoll_ret <= 0) {
      continue;
    }
    if (event.data.fd == stop_fd) {
      break;
    }

    int fd = accept(connection_fd, NULL, NULL);
    conditional_handle_error(fd == -1, "can't create socket descriptor");

    log("client connected", NULL);

    while (decode_commands(fd)) {
    }

    log("client disconnected", NULL);

    shutdown(fd, SHUT_RDWR);
    close(fd);
  }

  close(epoll_fd);

  shutdown(connection_fd, SHUT_RDWR);
  close(connection_fd);

  return 0;
}

//int init_server(long port) {
//  int fds[2];
//  conditional_handle_error(pipe(fds) == -1, "pipe error");
//  return server_loop(port, fds[0]);
//}

int init_server(long port) {
  sigset_t full_mask;
  sigfillset(&full_mask);
  sigprocmask(SIG_BLOCK, &full_mask, NULL);

  int fds[2];
  conditional_handle_error(pipe(fds) == -1, "pipe error");

  int child_pid = fork();
  conditional_handle_error(child_pid < 0, "fork error");
  if (child_pid == 0) {
    close(fds[1]);
    server_loop(port, fds[0]);
    close(fds[0]);

    exit(0);
  } else {
    close(fds[0]);

    log("pid %d", getpid());

    while (true) {
      siginfo_t info;
      sigwaitinfo(&full_mask, &info);
      int received_signal = info.si_signo;

      if (received_signal == SIGCHLD) {
        log("got SIGCHLD, stopping...", NULL);
        close(fds[1]);
        break;
      }
      if (received_signal == SIGTERM ||
          received_signal == SIGINT) {
        log("got signal, stopping..", NULL);

        ssize_t written = write(fds[1], "!", 1);
        conditional_handle_error(written != 1, "writing failed");
        close(fds[1]);
        break;
      }
    }

    int status;
    conditional_handle_error(waitpid(child_pid, &status, 0) == -1,
                             "wait error");
  }

  return 0;
}
