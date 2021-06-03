#ifndef __VFS_H
#define __VFS_H

#include <stddef.h>
#include "allocator.h"
#include "thread.h"

#define MAX_PATH 512
#define O_CREAT 0b1

struct filesystem_list {
    struct filesystem_list *next;
    struct mount *fs_mount;
};

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
    int (*write)(struct file *file, const void *buf, size_t len);
    int (*read)(struct file *file, void *buf, size_t len);
    size_t (*filesize)(struct file *file);
    void (*file_name)(struct vnode *vnode, char *buf);
    void *(*content)(struct vnode *vnode);
};

struct vnode_operations {
    int (*lookup)(struct vnode *dir_node, struct vnode **target, const char *component_name);
    int (*create_file)(struct vnode *dir_node, struct vnode **target, const char *component_name);
    int (*create_dir)(struct vnode *dir_node, const char *component_name);
    void (*list)(struct vnode *dir_node);
    void (*dir_name)(struct vnode *vnode, char *buf);
    void *(*content)(struct vnode *vnode);
    void (*set_parent)(struct vnode *mount, struct vnode *mountpoint);
};

static struct vnode *vfs_find_vnode(const char *pathname, struct vnode *root);
int register_filesystem(struct filesystem *fs);
struct file *fopen(const char *pathname, int flags);
struct file *vfs_fopen(const char *pathname, int flags, struct vnode *root);
int fclose(struct file *file);
int fwrite(struct file *file, const void *buf, size_t len);
int fread(struct file *file, void *buf, size_t len);
int fseek(struct file *fp, int offset, int whence);
int ftell(struct file *fp);
void rewind(struct file *fp);
int mkdir(const char *pathname);
int vfs_mkdir(const char *pathname, struct vnode *root);
int ls(const char *pathname);
int vfs_ls(const char *pathname, struct vnode *root);
int cat(const char *pathname);
int vfs_cat(const char *pathname, struct vnode *root);

int vfs_mount(const char *device, const char *mountpoint, const char *filesystem);
int vfs_umount(const char *mountpoint);

struct mount *get_mount(const char *filesystem);
char *get_pwd();

int cd(const char *pathname);

#endif
