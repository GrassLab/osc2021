#include "vfs_test.h"
#include <vfs.h>
#include <tmpfs.h>
#include <printf.h>
#include <string.h>

void vfs_open_test() {
  struct file* _file;
  struct tmpfs_inode *inode;
  
  _file = vfs_open("/flag.txt", 0);
  if(_file == null) {
    printf("null\n");
    return;
  }
  inode = _file->vnode->internal;
  printf("file name: %s\n", inode->name);
  vfs_close(_file);
}

void vfs_read_test() {
  struct file* _file;
  char buf[1025];
  size_t len, size;
  len = 1025;
  
  _file = vfs_open("/user/lib/string.c", 0);
  
  if(_file == null) {
    printf("null\n");
    return;
  }
  
  _file->f_pos = 512;

  size = vfs_read(_file, buf, len);

  printf("read bytes: %d %s\n", size, buf);

  vfs_close(_file);
}

void vfs_write_test() {
  struct file* _file;
  char buf[1025];
  size_t len, size;
  len = 12;

  _file = vfs_open("/test", O_CREAT);
  
  if(_file == null) {
    printf("null\n");
    return;
  }
  
  strncpy(buf, "test_test_1", len);

  size = vfs_write(_file, buf, len);

  printf("write bytes: %d\n", size);

  vfs_close(_file);

  _file = vfs_open("/test", 0);
  
  if(_file == null) {
    printf("null\n");
    return;
  }
  
  buf[0] = '\0';
  size = vfs_read(_file, buf, len);

  printf("read bytes: %d %s\n", size, buf);

  vfs_close(_file);

}