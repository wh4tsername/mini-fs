#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#include "handlers/decode_execute.h"
#include "constants/fs_constants.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Denis Pominov");
MODULE_DESCRIPTION("Mini filesystem Linux module.");
MODULE_VERSION("1.0");

struct mini_fs_device {
  char* memory;
  char* data;
  char* execute_results;
  struct semaphore sem;
} virtual_device;

struct cdev* mcdev;
int major_number;
int ret;

dev_t dev_num;

#define DEVICE_NAME "mini-fs"

int device_open(struct inode* inode, struct file* filp) {
  if (down_interruptible(&virtual_device.sem) != 0) {
    printk(KERN_ALERT "mini-fs: could not lock device during open");
    return -1;
  }

  printk(KERN_INFO "mini-fs: opened device");
  return 0;
}

ssize_t device_read(struct file* filp, char* buffer, size_t size,
                    loff_t* offset) {
  printk(KERN_INFO "mini-fs: reading from device");

  ret = copy_to_user(buffer, virtual_device.data, size);

  if (ret < 0) {
    return ret;
  }

  if (decode_execute(virtual_device.data, virtual_device.execute_results) < 0) {
    return -1;
  }

  return ret;
}

ssize_t device_write(struct file* filp, const char* buffer, size_t size,
                     loff_t* offset) {
  printk(KERN_INFO "mini-fs: writing to device");

  ret = copy_from_user(virtual_device.execute_results, buffer, size);

  return ret;
}

int device_close(struct inode* inode, struct file* filp) {
  up(&virtual_device.sem);

  printk(KERN_INFO "mini-fs: closed device");

  return 0;
}

struct file_operations fops = {.owner = THIS_MODULE,
                               .open = device_open,
                               .release = device_close,
                               .write = device_write,
                               .read = device_read};

static int driver_entry(void) {
  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

  if (ret < 0) {
    printk(KERN_ALERT "mini-fs: failed to allocate a major number");
    return ret;
  }

  major_number = MAJOR(dev_num);
  printk(KERN_INFO "mini-fs: major number is %d", major_number);
  printk(KERN_INFO "\tuse \"mknod /dev/%s c %d 0\" for device file",
         DEVICE_NAME, major_number);

  mcdev = cdev_alloc();
  mcdev->ops = &fops;
  mcdev->owner = THIS_MODULE;

  ret = cdev_add(mcdev, dev_num, 1);

  if (ret < 0) {
    printk(KERN_ALERT "mini-fs: unable to add cdev to kernel");
    return ret;
  }

  sema_init(&virtual_device.sem, 1);

  virtual_device.memory = kmalloc(FS_SIZE, GFP_KERNEL);
  virtual_device.data = kmalloc(FS_SIZE, GFP_KERNEL);
  virtual_device.execute_results = kmalloc(FS_SIZE, GFP_KERNEL);

  return 0;
}

static void driver_exit(void) {
  kfree(virtual_device.memory);
  kfree(virtual_device.execute_results);
  kfree(virtual_device.data);

  cdev_del(mcdev);

  unregister_chrdev_region(dev_num, 1);
  printk(KERN_INFO "mini-fs: unload module");
}

module_init(driver_entry);
module_exit(driver_exit);
