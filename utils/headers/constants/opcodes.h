#ifndef MINI_FS_UTILS_HEADERS_CONSTANTS_OPCODES_H_
#define MINI_FS_UTILS_HEADERS_CONSTANTS_OPCODES_H_

enum OPCODE : char
{
  FS_QUIT = 'a',
  FS_INIT,
  FS_DESTROY,
  FS_LS,
  FS_MKDIR,
  FS_RM,
  FS_TOUCH,
  FS_OPEN,
  FS_CLOSE,
  FS_SEEK,
  FS_WRITE,
  FS_READ
};

#endif //MINI_FS_UTILS_HEADERS_CONSTANTS_OPCODES_H_
