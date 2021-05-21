#include "vfs.h"

#include "alloc.h"
#include "cpio.h"
#include "printf.h"
#include "string.h"
#include "tmpfs.h"

void vfs_test() {
  struct file* a = vfs_open("hello", O_CREAT);
  struct file* b = vfs_open("world", O_CREAT);
  if (a != 0 && b != 0) {
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
  }
  b = vfs_open("hello", 0);
  a = vfs_open("world", 0);
  if (a != 0 && b != 0) {
    int sz;
    char buf[200];
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("%s\n", buf);  // should be Hello World!
  }
  vfs_close(a);
  vfs_close(b);

  printf("\nfile1.txt\n");
  a = vfs_open("file1.txt", 0);
  if (a != 0) {
    int sz;
    char buf[200];
    sz = vfs_read(a, buf, 100);
    buf[sz] = '\0';
    printf("%s\n", buf);
  }
  vfs_close(a);

  printf("\nfile2.txt\n");
  a = vfs_open("file2.txt", 0);
  if (a != 0) {
    int sz;
    char buf[200];
    sz = vfs_read(a, buf, 100);
    buf[sz] = '\0';
    printf("%s\n", buf);
  }
  vfs_close(a);
}

void vfs_init() {
  tmpfs_init();
  rootfs = (struct mount*)malloc(sizeof(struct mount));
  struct filesystem fs;
  fs.name = "tmpfs";
  register_filesystem(&fs);
  fs.setup_mount(&fs, rootfs);
  current_dir = rootfs->root;
  cpio_populate_rootfs();
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  if (strcmp(fs->name, "tmpfs") == 0) {
    fs->setup_mount = tmpfs_setup_mount;
    return 1;
  }
  return -1;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  struct vnode* target;
  struct file* fd = 0;
  if (!strcmp(pathname, "/") && flags != O_CREAT) {
    target = rootfs->root;
    fd = (struct file*)malloc(sizeof(struct file));
    fd->vnode = target;
    fd->f_ops = target->f_ops;
    fd->f_pos = 0;
  } else if (!strcmp(pathname, ".") && flags != O_CREAT) {
    target = current_dir;
    fd = (struct file*)malloc(sizeof(struct file));
    fd->vnode = target;
    fd->f_ops = target->f_ops;
    fd->f_pos = 0;
  } else if (flags == O_CREAT) {
    int found = current_dir->v_ops->lookup(current_dir, &target, pathname);
    if (!found) {
      current_dir->v_ops->create(current_dir, &target, pathname);
      fd = (struct file*)malloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    } else {
      printf("File already exists!!\n");
    }
  } else {
    int found = current_dir->v_ops->lookup(current_dir, &target, pathname);
    if (!found) {
      printf("File not found!!\n");
    } else {
      fd = (struct file*)malloc(sizeof(struct file));
      fd->vnode = target;
      fd->f_ops = target->f_ops;
      fd->f_pos = 0;
    }
  }
  return fd;
}

int vfs_close(struct file* file) {
  // 1. release the file descriptor
  free(file);
  return 1;
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
