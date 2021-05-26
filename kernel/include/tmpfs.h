#ifndef _TMPFS_H
#define _TMPFS_H

#include "def.h"
#include "vfs.h"
struct tmpfs_internal {
    char name[128];
    struct vnode *l_child; // left most child
    struct vnode *r_sibling; // right next sibling

    void *start_addr;
    int size;
};

int tmpfs_setup(struct filesystem *fs, struct mount* mount);
int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_write (struct file *file, const void *buf, size_t len);
int tmpfs_read (struct file *file, void *buf, size_t len);
struct vnode *create_tmpfs_vnode(struct vnode *parent, const char *name);
struct tmpfs_internal *create_tmpfs_vnode_internal(const char *name);
// void append_child(struct vnode *parent, struct vnode *node);

#endif