#include "vfs_test.h"
#include <vfs.h>
#include <tmpfs.h>
#include <printf.h>

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
  
  _file->f_pos = 513;

  size = vfs_read(_file, buf, len);

  printf("read bytes: %d %s\n", size, buf);

  vfs_close(_file);
}