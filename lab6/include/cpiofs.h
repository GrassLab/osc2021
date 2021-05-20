# include "vfs.h"

# define CPIOFS_DEFAULT_MODE   4
//# define TMPFS_MAX_SIZE      500

struct cpiofs_internal{
  size_t size;
  char *content;
};

//void tmpfs_get_mount_fs(struct mount* mount_point);
struct filesystem* cpiofs_get_fs();
int cpiofs_setup_mount(struct filesystem* fs, struct mount* mount);
int cpiofs_mkdir(struct vnode* dir_name, struct vnode** target, const char* component_name);
int cpiofs_create(struct vnode* dir_name, struct vnode** target, const char* component_name);
int cpiofs_read(struct file* file, void* buf, size_t len);
int cpiofs_write(struct file* file, const void* buf, size_t len);
int cpiofs_file_cat(struct vnode *vnode);
int cpiofs_dir_cat(struct vnode *vnode);
int cpiofs_get_size(struct vnode *vnode);
