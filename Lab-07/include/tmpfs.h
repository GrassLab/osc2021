#ifndef	_TMPS_H
#define	_TMPS_H
#include "vfs.h"

int tmpfs_write(struct file* file, const void* buf, int len);
int tmpfs_read(struct file* file, void* buf, int len);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name, int type);
int tmpfs_setup_mount(struct filesystem* fs, struct mount* mount);
void setup_tmpfs(struct filesystem **fs, char *name);

#endif