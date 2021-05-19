#ifndef _VFS_H
#define _VFS_H

typedef struct _vnode {
  struct _mount* mount;
  struct _vnode_operations* v_ops;
  struct _file_operations* f_ops;
  void* internal;
}vnode;

typedef struct _file {
  vnode* vnode;
  int f_pos; // The next read/write position of this opened file
  struct file_operations* f_ops;
  int flags;
}file;

typedef struct _mount {
  vnode* root;
  struct _filesystem* fs;
}mount;

typedef struct _filesystem {
  const char* name;
  int (*setup_mount)(struct _filesystem* fs, mount* mount);
}filesystem;

typedef struct _file_operations {
  int (*write) (file* file, const void* buf, int len);
  int (*read) (file* file, void* buf, int len);
}file_operations;

typedef struct _vnode_operations {
  int (*lookup)(vnode* dir_node,  vnode** target,char* component_name);
  int (*create)(vnode* dir_node,  vnode** target,char* component_name);
}vnode_operations;

mount* rootfs;


file* vfs_open(const char* pathname, int flags);

int vfs_close(file* file);
int vfs_write(file* file, const void* buf, int len);
int vfs_read(file* file, void* buf, int len);
int vfs_init(void* setup_mount_f,void* write_f, void* read_f );

#endif
