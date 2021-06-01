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

void vfs_read_test(char* filename) {
  struct file* _file;
  size_t len, size;
  len = 4096;
  char buf[len];
 
  
  
  _file = vfs_open(filename, 0);
  
  if(_file == null) {
    printf("null\n");
    return;
  }
  
  _file->f_pos = 0;

  size = vfs_read(_file, buf, len);
  buf[size] = '\0';
  printf("%s\nread bytes: %d\n", buf, size);
  
  vfs_close(_file);
}

void vfs_write_test(char* filename, char* buf, size_t len) {
  struct file* _file;
  size_t size;
  _file = vfs_open(filename, 1);
  
  if(_file == null) {
    printf("null\n");
    return;
  }
  
  _file->f_pos = 0;

  size = vfs_write(_file, buf, len);

  printf("write bytes: %d\n", size);

  vfs_close(_file);

}