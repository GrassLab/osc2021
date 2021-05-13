# include "vfs.h"

struct mount* rootfs;

int register_filesystem(struct filesystem* fs) {
  return 0;
  // register the file system to the kernel.
}

struct file* vfs_open(const char* pathname, int flags) {
  return 0;
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
}
int vfs_close(struct file* file) {
  return 0;
  // 1. release the file descriptor
}
int vfs_write(struct file* file, const void* buf, size_t len) {
  return 0;
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, size_t len) {
  return 0;
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}
