#ifndef	_VFS_H
#define	_VFS_H

#include "mm.h"

#define REG_FILE 1
#define REG_DIR  2
#define O_CREAT  4
#define MAX_PATH_LEN 512U

struct direntry {
  char *name;
  struct vnode* entry;
  struct direntry *next;
};

struct directory {
  struct direntry * head;
  struct direntry * tail;
  void *internal;
};

struct mount {
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem {
  char *name;
  int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
  int (*write) (struct file* file, const void* buf, int len);
  int (*read) (struct file* file, void* buf, int len);
};

struct file {
  struct vnode* vnode;
  int f_pos; // The next read/write position of this file descriptor
  struct file_operations* f_ops;
  int flags;
};

struct vnode {
  struct mount* mount;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  int type;
  void* internal;
};

struct vnode_operations {
  int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
  int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name, int type);
};

void init_fs();
void register_filesystem(struct filesystem* fs, char *fs_name);
struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file * file);
int vfs_write(struct file* file, const void* buf, int len);
int vfs_read(struct file* file, void* buf, int len);
int user_open(const char* pathname, int flags);
int user_read(int file_index,void* buf, int len);
int user_write(int file_index, const void* buf, int len);
void user_close(int file_index);
void vfs_ls(const char* pathname, int flags);
#endif  /*_VFS_H */