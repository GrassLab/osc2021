#include "vfs.h"
#include <types.h>
#include <printf.h>
#include <varied.h>

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  int i;
  for(i = 0; i < REG_FS_NUM; i++) {
    if(registed_fs[i].name == null) {
      registed_fs[i] = *fs;
      return 0;
      break;
    }
  }
  return -1;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  return null;
}
int vfs_close(struct file* file) {
  // 1. release the file descriptor
  return 0;
}
int vfs_write(struct file* file, const void* buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file* file, void* buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
  return file->f_ops->read(file, buf, len);
}

void root_fs_init() {
  int err;
  //init tmpfs
  extern void* tmpfs_init();
  
  tmpfs_init();

  rootfs = (struct mount*)varied_malloc(sizeof(struct mount));

  if(rootfs == null)
    return;
  
  err = registed_fs[0].setup_mount(&registed_fs[0], rootfs);
  
  if(err == -1) {
    printf("mount root fs error\n");
    return;
  }
  extern void tmpfs_load_initramfs(struct mount* mount);

  tmpfs_load_initramfs(rootfs); 
}