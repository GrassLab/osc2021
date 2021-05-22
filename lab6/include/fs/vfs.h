#ifndef __FS_VFS_H_
#define __FS_VFS_H_

#include <list.h>
#include <stddef.h>
#include <file.h>

typedef long ssize_t;

struct vnode {
    struct mount* mnt;
    struct vnode *parent;
    struct list_head subnodes;
    struct list_head nodes;
    size_t flags; /* TODO: deprecated */
    char *name;
    /* TODO: it should be received by some v_ops, but I'm too lazy now */
    unsigned int f_mode;
    struct file_operations* f_ops;
    struct vnode_operations* v_ops;
    size_t size;
    void *internal; /* internal data structure impl by each fs */
};

struct mount {
    struct vnode* root;
    struct filesystem* fs;
};

struct filesystem {
    const char *name;
    int (*init_fs)(struct filesystem *);
    int (*vnode_mount)(struct vnode *);
    struct filesystem *next;
};

/* TODO: add flags arg for each ops */
struct vnode_operations {
    /* only lookup a child node of the provided vnode */
    int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
    int (*create)(struct vnode* dir_node, const char* component_name);
    int (*unlink)(struct vnode* dir_node, const char* component_name);
    int (*mkdir)(struct vnode* dir_node, const char* component_name);
    int (*rmdir)(struct vnode* dir_node, const char* component_name);
};

struct file_operations {
    ssize_t (*write) (struct file* file, const void* buf, size_t len);
    ssize_t (*read) (struct file* file, void* buf, size_t len);
};

int register_filesystem(struct filesystem* fs);
struct vnode *traverse_path(const char *pathname, int last_dir);
struct vnode *recursive_build_dir(const char *pathname);
struct file* vfs_open(const char *pathname, int flags);
ssize_t vfs_read(struct file *file, void *buf, size_t len);
ssize_t vfs_write(struct file *file, const void *buf, size_t len);
int vfs_close(struct file *file);

struct vnode *vfs_lookup(struct vnode* dir_node, const char *component_name);
int vfs_mkdir(struct vnode* dir_node, const char *component_name);
int vfs_rmdir(struct vnode* dir_node, const char *component_name);
int vfs_unlink(struct vnode* dir_node, const char *component_name);

void init_rootfs();

#endif