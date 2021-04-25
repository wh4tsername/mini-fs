#include <defines.h>

#include "client/client.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    panic("file system server address should be specified as an argument");
  }

  const char* address_string = argv[1];
  return client_main(address_string);
}
