# ifndef __VFS__
# define __VFS__
# include "list.h"
# define size_t unsigned long
# define VFS_FILENAME_MAX_LEN 128

# define VISFILE(v) ((v)->dentry->type == FILE)
# define VISDIR(v)  ((v)->dentry->type == DIR)


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
  struct file* file;
};

struct file{
  struct vnode* vnode;
  size_t f_pos;
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
  int (*cat)(struct vnode* dir_node);
};

void vfs_init();

int register_filesystem(struct filesystem* fs);

void vfs_list_tree();
int vfs_do_mkdir(char *name, struct vnode *dir_node);


struct dentry* vfs_create_dentry(struct dentry* parent, const char* name, enum dentry_type type);
int vfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
struct vnode* get_root_vnode();
int get_pwd_string(struct vnode *v, char *s);
int vfs_split_path(char *path, char ***list);
int get_vnode_by_path(struct vnode *dir_node, struct vnode **target, char *path);
int do_cd(char *path);
void do_ls(char *path);
void do_cat(char *path);
int do_open(const char *pathname, int flags);
int do_close(int fd);
int do_write(int fd, const void* buf, size_t len);
int do_read(int fd, void* buf, size_t len);

void sys_open(struct trapframe *arg);
void sys_close(struct trapframe *arg);
void sys_write(struct trapframe *arg);
void sys_read(struct trapframe *arg);
# endif
