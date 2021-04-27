#ifndef MINI_FS_FS_MODULE_MODULE_DEFINES_H_
#define MINI_FS_FS_MODULE_MODULE_DEFINES_H_

#include <linux/kernel.h>

//#define cond_server_panic(stmt, msg)   \
//  if (stmt) {                          \
//    printk(KERN_ALERT "SERVER LOG: "); \
//    printk(KERN_ALERT "%s\n", msg);    \
//    return -1;                         \
//  }
//
//#define server_panic(msg)              \
//  {                                    \
//    printk(KERN_ALERT "SERVER LOG: "); \
//    printk(KERN_ALERT "%s\n", msg);    \
//    return -1;                         \
//  }

#define cond_server_panic(stmt, msg)   \
  if (stmt) {                          \
    return -1;                         \
  }

#define server_panic(msg)              \
  {                                    \
    return -1;                         \
  }

#define server_log2(format, arg)    \
  printk(KERN_INFO "SERVER LOG: "); \
  printk(KERN_INFO format, arg);    \
  printk(KERN_INFO "\n");

#define server_log1(msg)            \
  printk(KERN_INFO "SERVER LOG: "); \
  printk(KERN_INFO msg);            \
  printk(KERN_INFO "\n");

#define check_ret_code(stmt) \
  if ((stmt) < 0) {          \
    return -1;               \
  }

#endif  // MINI_FS_FS_MODULE_MODULE_DEFINES_H_
