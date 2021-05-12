#ifndef _VFS_H
#define _VFS_H

#include "def.h"

#define O_CREAT (0)

struct vnode {
    struct mount *mount;
    struct vnode_operations *v_ops;
    struct file_operations *f_ops;
    void *internal;
};

struct file {
    struct vnode *vnode;
    size_t f_pos; // The next read/write position of this opened file
    struct file_operations *f_ops;
    int flags;
};

struct mount {
    struct vnode *root;
    struct filesystem *fs;
};

struct filesystem {
    const char *name;
    int (*setup_mount)(struct filesystem *fs, struct mount *mount);
};

struct file_operations {
    int (*write) (struct file *file, const void *buf, size_t len);
    int (*read) (struct file *file, void *buf, size_t len);
};

struct vnode_operations {
    int (*lookup)(struct vnode *dir_node, struct vnode **target, const char *component_name);
    int (*create)(struct vnode *dir_node, struct vnode **target, const char *component_name);
};

struct filesystem_node {
    struct filesystem *fs;
    struct filesystem_node *next_node;
};

struct filesystem_list {
    int count;
    struct filesystem_node *head;
    struct filesystem_node *tail;
};

void init_root(const char *name);
int register_filesystem(struct filesystem* fs);
struct filesystem *get_filesystem(const char *fs_name);

struct file* vfs_open(const char* pathname, int flags);
int vfs_read(struct file* file, void* buf, size_t len);
int open(const char* pathname, int flags);

#endif