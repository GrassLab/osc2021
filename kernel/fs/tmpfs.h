#ifndef TMPFS_H
#define TMPFS_H

#include "vfs.h"

#define TMPFS_SIZE 256

struct tmpfs_internel {
    char name[128];

    void * addr;
    int size;

    struct vnode * next_sibling;
};

int tmpfs_setup(struct filesystem * fs, struct mount * mount);
int tmpfs_lookup(struct vnode * dir_node, struct vnode ** target, const char * component_name);
int tmpfs_create(struct vnode * dir_node, struct vnode ** target, const char * component_name);
int tmpfs_read(struct file * file, void * buf, size_t len);
int tmpfs_write(struct file * file, const void * buf, size_t len);

struct vnode * tmpfs_create_vnode(struct vnode * src, const char * name);
struct tmpfs_internel * tmpfs_create_internel(const char * name);

void tmpfs_append_child(struct vnode * parent, struct vnode * child);

#endif