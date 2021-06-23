#ifndef VFS_H
#define VFS_H
#include "data_type.h"

struct mount* rootfs;
// TODO: add dentry in the future

struct vnode;

struct vnode_child {
    struct vnode *child;
    struct vnode_child *next;
};

enum vnode_type {
    VNODE_DIR,
    VNODE_FILE,
};

struct vnode {
    struct mount* mount;
    struct vnode_operations* v_ops;
    struct file_operations* f_ops;
    struct vnode_child *childs;
    char *name;
    u32 type;
    void* internal;
};

enum FILE_FLAG {
    F_CLOSE,
    O_READ,
    O_CREAT,
};

struct file {
    struct vnode* vnode;
    u32 f_pos; // The next read/write position of this opened file
    struct file_operations* f_ops;
    int flags;
    void *internal;
};

struct mount {
    struct vnode* root;
    struct filesystem* fs;
};

struct filesystem {
    const char* name;
    int (*setup_mount)(struct filesystem* fs, struct mount* mount);
};

struct file_operations {
    int (*write) (struct file* file, const void* buf, u32 len);
    int (*read) (struct file* file, void* buf, u32 len);
    int (*flush) (struct file *file);
};

struct vnode_operations {
    int (*lookup)(struct vnode* dir_node, struct vnode** target, const char* component_name);
    int (*create)(struct vnode* dir_node, struct vnode** target, const char* component_name);
    int (*flush)(struct vnode *node);
};

void init_vfs ();
void vfs_ls (const char *path);
int vfs_touch (const char *path);
int vfs_mkdir (const char *path);
int vfs_write(struct file* file, const void* buf, u32 len);
int vfs_read(struct file* file, void* buf, u32 len);
int vfs_close(struct file* file);
struct file* vfs_open(const char* pathname, int flags);
int register_filesystem(struct filesystem* fs);
#endif
