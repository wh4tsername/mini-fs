#include "net/loop.h"

#include <defines.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    panic("available port should be specified as an argument");
  }

  long port = strtol(argv[1], NULL, 10);
  return init_server(port);
}
