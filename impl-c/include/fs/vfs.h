#pragma once
#include <stddef.h>

struct vnode {
  struct mount *mnt;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  void *internal;
};

struct file {
  struct vnode *node;
  unsigned long f_pos; // The next read/write position of this opened file
  struct file_operations *f_ops;
  int flags;
};

struct mount {
  struct vnode *root;
  struct filesystem *fs;
};

struct filesystem {
  const char *name;

  // Method to call while mounting a node in vfs within this filesystem
  int (*setup_mount)(struct filesystem *fs, struct mount *mnt);

  // Available file systems will be linked inside os after initialized
  struct filesystem *next;
};

struct file_operations {
  int (*write)(struct file *f, const void *buf, unsigned long len);
  int (*read)(struct file *f, void *buf, unsigned long len);
};

struct vnode_operations {
  int (*lookup)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
  int (*create)(struct vnode *dir_node, struct vnode **target,
                const char *component_name);
};

extern struct mount *rootfs;

// register the file system to the kernel.
int register_filesystem(struct filesystem *fs);

// 1. Lookup pathname from the root vnode.
// 2. Create a new file descriptor for this vnode if found.
// 3. Create a new file if O_CREAT is specified in flags.
struct file *vfs_open(const char *pathname, int flags);

// 1. release the file descriptor
int vfs_close(struct file *file);

// 1. write len byte from buf to the opened file.
// 2. return written size or error code if an error occurs.
int vfs_write(struct file *file, const void *buf, size_t len);

// 1. read min(len, readable file data size) byte to buf from the opened file.
// 2. return read size or error code if an error occurs.
int vfs_read(struct file *file, void *buf, size_t len);

void vfs_init();