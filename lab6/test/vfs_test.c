#include "vfs_test.h"
#include <vfs.h>
#include <tmpfs.h>
#include <printf.h>

void vfs_test() {
  struct file* _file;
  struct tmpfs_inode *inode;
  
  _file = vfs_open("/usr/lib/test", 1);
  if(_file == null) {
    printf("null\n");
    return;
  }
  inode = _file->vnode->internal;
  printf("file name: %s\n", inode->name);
}