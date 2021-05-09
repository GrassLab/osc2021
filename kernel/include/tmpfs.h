#ifndef _TMPFS_H
#define _TMPFS_H

#include "def.h"

struct tmpfs_internal {
    char name[128];
    struct vnode *l_child; // left most child
    struct vnode *r_sibling; // right next sibling
};

int tmpfs_setup(struct filesystem* fs, struct mount* mount);
int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_write (struct file* file, const void* buf, size_t len);
int tmpfs_read (struct file* file, void* buf, size_t len);

struct tmpfs_internal *create_tmpfs_vnode_internal(const char *name);

#endif