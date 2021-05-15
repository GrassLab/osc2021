# include "vfs.h"

# define TMPFS_DEFAULT_MODE 6

void tmpfs_get_mount_fs(struct mount* mount_point);
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
int tmpfs_mkdir(struct vnode* dir_name, struct vnode** target, const char* component_name);
