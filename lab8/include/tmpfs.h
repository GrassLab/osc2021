# include "vfs.h"

# define TMPFS_DEFAULT_MODE 6
# define TMPFS_MAX_SIZE     500

struct tmpfs_internal{
  size_t size;
  char content[TMPFS_MAX_SIZE+1];
};

struct filesystem* tmpfs_get_fs();
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_unmount(struct mount *mount);
int tmpfs_mkdir(struct vnode* dir_name, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_name, struct vnode** target, const char* component_name);
int tmpfs_read(struct file* file, void* buf, size_t len);
int tmpfs_write(struct file* file, const void* buf, size_t len);
int tmpfs_file_cat(struct vnode *vnode);
int tmpfs_dir_cat(struct vnode *vnode);
int tmpfs_get_size(struct vnode *vnode);
int tmpfs_dir_rm(struct vnode *vnode);
int tmpfs_file_rm(struct vnode *vnode);
