#include "vfs.h"

#include "alloc.h"
#include "cpio.h"
#include "printf.h"
#include "string.h"
#include "thread.h"
#include "tmpfs.h"

void vfs_test() {
  const char* vfs_argv[] = {"vfs_test", 0};
  exec("vfs_test", vfs_argv);
}

void vfs_ls_test() {
  const char* ls_argv[] = {"ls", "/", 0};
  exec("ls", ls_argv);
}

void vfs_init() {
  fs_list.head = 0;
  fs_list.tail = 0;
  // init and register tmpfs
  tmpfs_init();
  struct filesystem* tmpfs =
      (struct filesystem*)malloc(sizeof(struct filesystem));
  tmpfs->name = "tmpfs";
  tmpfs->setup_mount = tmpfs_setup_mount;
  register_filesystem(tmpfs);
  // use tmpfs to mount root filesystem
  rootfs = (struct mount*)malloc(sizeof(struct mount));
  struct filesystem* fs = get_fs_by_name("tmpfs");
  if (fs == 0) {
    printf("Mount root filesystem failed!!\n");
    return;
  }
  fs->setup_mount(fs, rootfs);
  current_dir = rootfs->root;
  cpio_populate_rootfs();
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  if (fs_list.head == 0) {
    fs_list.head = fs;
    fs_list.head->next = 0;
    fs_list.tail = fs_list.head;
  } else {
    fs_list.tail->next = fs;
    fs_list.tail = fs_list.tail->next;
  }
  return 1;
}

struct filesystem* get_fs_by_name(const char* name) {
  for (struct filesystem* fs = fs_list.head; fs != 0; fs = fs->next) {
    if (!strcmp(fs->name, name)) {
      return fs;
    }
  }
  printf("Filesystem \"%s\" not found!!\n", name);
  return 0;
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
      printf("File \"%s\" already exists!!\n", pathname);
    }
  } else {
    int found = current_dir->v_ops->lookup(current_dir, &target, pathname);
    if (!found) {
      printf("File \"%s\" not found!!\n", pathname);
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

int vfs_list(struct file* file, void* buf, int index) {
  return file->f_ops->list(file, buf, index);
}
