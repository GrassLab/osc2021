#ifndef _VFS_H
#define _VFS_H

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
};

struct file {
  struct vnode* vnode;
  size_t f_pos; // The next read/write position of this opened file
  struct file_operations* f_ops;
  int flags;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write) (struct file* file, const void* buf, size_t len);
  int (*read) (struct file* file, void* buf, size_t len);
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

struct mount* rootfs;

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
}
int vfs_close(struct file* file) {
  // 1. release the file descriptor
}
int vfs_write(struct file* file, const void* buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, size_t len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}

#endif
