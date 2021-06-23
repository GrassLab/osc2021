#ifndef TMPFS_H
#define TMPFS_H
#include "vfs.h"

int tmpfs_write (struct file* file, const void* buf, u32 len);
int tmpfs_read (struct file* file, void* buf, u32 len);
int tmpfs_lookup (struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create (struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_file_flush (struct file *file);
int tmpfs_vnode_flush (struct vnode *node);

#endif
