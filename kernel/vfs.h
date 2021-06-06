#ifndef VFS_H
#define VFS_H

#include "../lib/type.h"

struct vnode {
    struct mount * mount;
    struct vnode_operations * v_ops;
    struct file_operations * f_ops;
    void * internel;
};

struct file {
    struct vnode * vnode;
    size_t f_pos;   // The next read/write position of the opened file
    struct file_operations * f_ops;
    int flags;
};

struct mount {
    struct vnode * root;
    struct filesystem * fs;
};

struct filesystem {
    const char * name;
    int (*setup_mount)(struct filesystem * fs, struct mount * mount);
};

struct file_operations {
    int (*write) (struct file * file, const void * buf, size_t len);
    int (*read) (struct file * file, void * buf, size_t len);
};

struct vnode_operations {
    int (*lookup)(struct vnode * dir_node, struct vnode ** target, const char * component_name);
    int (*create)(struct vnode * dir_node ,struct vnode ** target, const char * component_name);
};

void init_filesystem();

int register_filesystem(struct filesystem * fs);

struct file * vfs_open(const char * path_name, int flags);

int vfs_close(struct file * file);
int vfs_write(struct file * file, const void * buf, size_t len);
int vfs_read(struct file * file, void * buf, size_t len);

void vfs_test(int test_id);

#endif