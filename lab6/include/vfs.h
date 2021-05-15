# ifndef __VFS__
# define __VFS__
# include "list.h"
# define size_t unsigned long
# define VFS_FILENAME_MAX_LEN 128

enum dentry_type{
  FILE = 0,
  DIR = 1,
  SDIR = 2,
};

struct dentry{
  struct list_head list;
  struct list_head childs;
  struct vnode* vnode;
  struct dentry* parent;
  char name[VFS_FILENAME_MAX_LEN];
  enum dentry_type type;
};

struct vnode{
  struct mount* mount;
  int mode;
  struct vnode_operations* v_ops;
  struct file_operations* f_ops;
  void* internal;
  struct dentry* dentry;
};

struct file{
  struct vnode* vnode;
  size_t f_pos;
  struct file_operations* f_ops;
  int flag;
};

struct mount{
  struct vnode* root;
  struct filesystem* fs;
};

struct filesystem{
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
  int (*mkdir)(struct vnode* dir_node, struct vnode** target, const char* component_name);
};

void vfs_init();

int register_filesystem(struct filesystem* fs);

void vfs_list_tree();
void vfs_do_mkdir(char *name);

struct file* vfs_open(const char* pathname, int flags);
int vfs_close(struct file* file);
int vfs_write(struct file* file, const void* buf, size_t len);
int vfs_read(struct file* file, void* buf, size_t len);

struct dentry* vfs_create_dentry(struct dentry* parent, const char* name, enum dentry_type type);
int vfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
# endif
