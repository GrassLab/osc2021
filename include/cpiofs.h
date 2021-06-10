# include "vfs.h"

# define CPIOFS_DEFAULT_MODE   4

struct cpiofs_internal{
  size_t size;
  char *content;
};

struct filesystem* cpiofs_get_fs();
int cpiofs_setup_mount(struct filesystem* fs, struct mount* mount);
int cpiofs_unmount(struct mount *mount);
int cpiofs_mkdir(struct vnode* dir_name, struct vnode** target, const char* component_name);
int cpiofs_create(struct vnode* dir_name, struct vnode** target, const char* component_name);
int cpiofs_read(struct file* file, void* buf, size_t len);
int cpiofs_write(struct file* file, const void* buf, size_t len);
int cpiofs_file_cat(struct vnode *vnode);
int cpiofs_dir_cat(struct vnode *vnode);
int cpiofs_get_size(struct vnode *vnode);
int cpiofs_rm(struct vnode *vnode);
int cpiofs_dir_release(struct vnode *vnode);
int cpiofs_file_release(struct vnode *vnode);
