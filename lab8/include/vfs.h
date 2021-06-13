#ifndef _VFS_H
#define _VFS_H

#define O_CREAT 2
 
struct _filesystem my_filesystem;
struct _mount my_mount;
void* my_write_f;
void* my_read_f;

typedef struct _vnode {
  struct _mount* mount;
  struct _vnode_operations* v_ops;
  struct _file_operations* f_ops;
  void* internal;
}vnode;

typedef struct _file {
  vnode* v_node;
  int f_pos; // The next read/write position of this opened file
  struct _file_operations* f_ops;
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
  int (*write) (file* file, void* buf, int len);
  int (*read) (file* file, void* buf, int len);
  void (*sync)(file* file);
}file_operations;

typedef struct _vnode_operations {
  int (*lookup)(vnode* dir_node,  vnode** target,char* component_name);
  int (*create)(vnode* dir_node,  vnode** target,char* component_name);
}vnode_operations;

mount* rootfs;


file* vfsOpen(char* pathname, int flags);

int vfsClose(file* file);
int vfsWrite(file* file,void* buf, int len);
int vfsRead(file* file, void* buf, int len);
void vfsSync(file* file);
int vfsInit(void* setup_mount_f);

#endif
