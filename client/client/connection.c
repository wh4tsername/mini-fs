#include "connection.h"

#include <defines.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void parse_address_string(const char* address_string, char** host, long* port) {
  char* pos = strchr(address_string, ':');
  conditional_handle_error(pos == NULL, "invalid address string");

  *port = strtol(pos + 1, NULL, 10);

  long host_size = pos - address_string + 1;
  *host = malloc(host_size);

  memcpy(*host, address_string, host_size - 1);
  (*host)[host_size - 1] = '\0';
}

int establish_connection(const char* address_string) {
  long port = 0;
  char* host = NULL;
  parse_address_string(address_string, &host, &port);

  int sockd = socket(AF_INET, SOCK_STREAM, 0);
  conditional_handle_error(sockd == -1, "socket error");

  struct hostent* hostent = gethostbyname(host);
  in_addr_t* ip = (in_addr_t*)(*hostent->h_addr_list);

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_addr.s_addr = *ip;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  conditional_parse_errno(connect(sockd, (const struct sockaddr*)&addr,
    sizeof(struct sockaddr_in)) == -1);

  free(host);

  return sockd;
}

void close_connection(int sockd) { close(sockd); }
