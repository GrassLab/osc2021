#ifndef _VFS_H_
#define  _VFS_H_

#include "list.h"
#include "cpio.h"
#include "type.h"

#define MAX_NAME_SIZE 20

typedef struct dentry {
    struct mount *mount;
    struct vnode *prev;
    struct vnode *parent;
    list_head_t  sibling;
    list_head_t  subdir;
} dentry_t;

typedef struct vnode {
    char                    name[MAX_NAME_SIZE];
    struct dentry           dentry;
    struct vnode_operations *v_ops;
    struct file_operations   *f_ops;
    void                    *internal;
    list_head_t             node;
} vnode_t;

typedef struct file {
    uint16_t              use_num;
    size_t                f_pos;
    struct vnode          *vnode;
} file_t;

typedef struct mount{
    const char      *fs_name;
    struct vnode    *root;
} mount_t;

typedef struct vnode_operations {
    void (*create_file)(struct vnode *dir_node, struct vnode **target, const char *file_name);
    void (*create_dir)(struct vnode *dir_node, const char *dir_name);
} vnode_operations_t;

typedef struct file_operations {
    int (*write)(struct file *file, const void *buf, size_t len);
    int (*read) (struct file *file, void *buf, size_t len);
} file_operations_t;

extern mount_t *rootfs;

int vfs_dir_operation(const char*, vnode_t**, uint16_t);
int vfs_mount(const char*, const char*, const char*, vnode_t**);
int vfs_read(file_t*, void*, size_t);
int vfs_write(file_t*, const void*, size_t);
int vfs_close(file_t*);
file_t* vfs_open(const char*, int, vnode_t*);
int vfs_setup_mount(mount_t**, const char*, const char*);
void vfs_init(const char*);

#endif
