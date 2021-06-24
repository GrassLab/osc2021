#ifndef VFS_H
#define VFS_H

#define O_CREATE 1


typedef struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
} vnode;

typedef struct file {
  struct vnode* vnode;
  unsigned long f_pos; // The next read/write position of this opened file
  struct file_operations* f_ops;
  int flags;
} file;

typedef struct mount {
  struct vnode* root;
  struct filesystem* fs;
} mount;

typedef struct filesystem {
  const char* name;
  int (*setup_mount)(struct filesystem* fs, mount* mount);
} filesystem;

typedef struct file_operations {
  int (*write) (file* file, const void* buf, unsigned long len);
  int (*read) (file* file, void* buf, unsigned long len);
} file_operations;

typedef struct vnode_operations {
  int (*lookup)(vnode* dir_node, vnode** target, const char* component_name);
  int (*create)(vnode* dir_node, vnode** target, const char* component_name);
} vnode_operations;

int register_filesystem(filesystem* fs);
file* vfs_open(const char* pathname, int flags);
int vfs_close(file* file);
int vfs_write(file* file, const void* buf, unsigned long len);
int vfs_read(file* file, void* buf, unsigned long len);

#endif