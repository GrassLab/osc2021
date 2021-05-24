#include "fs/vfs.h"

#include "string.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_FS_VFS
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

struct mount *rootfs;

// filesystems registered
struct filesystem registered = {
    .name = "reserved-not-used",
    .setup_mount = NULL,
    .next = NULL,
};

void mount_root_fs();

void vfs_init() {}

void mount_root_fs() {
  uart_println("do mount root fs");
  ;
}

int register_filesystem(struct filesystem *fs) {
  struct filesystem *cur;
  for (cur = registered.next; cur != NULL; cur = cur->next) {
    if (strcmp(cur->name, fs->name) == 0) {
      return 1;
    }
  }

  // Register the file system into kernel.
  struct filesystem *_first = registered.next;
  registered.next = fs;
  fs->next = _first;

#ifdef CFG_LOG_FS_VFS
  log_println("Filesystems registered:");
  int i = 0;
  for (cur = registered.next, i = 0; cur != NULL; cur = cur->next, i++) {
    log_println("[%d]: %s", i, cur->name);
  }
#endif
  return 0;
}

struct file *vfs_open(const char *pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  return NULL;
}

int vfs_close(struct file *file) {
  // 1. release the file descriptor
  return 0;
}

int vfs_write(struct file *file, const void *buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return 0;
}

int vfs_read(struct file *file, void *buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
  return 0;
}
