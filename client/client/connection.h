#ifndef MINI_FS_CLIENT_CONNECTION_H_
#define MINI_FS_CLIENT_CONNECTION_H_

int establish_connection(const char* address_string);

void close_connection(int sockd);

#endif  // MINI_FS_CLIENT_CONNECTION_H_
