#include "vfs.h"
#include "tmpfs.h"

struct filesystem tmpfs = {
    .name = "tmpfs",
    .setup_mount = tmpfs_setup
};

struct vnode_operations tmpfs_v_ops = {
    .lookup = tmpfs_lookup,
    .create = tmpfs_create
};

struct file_operations tmpfs_f_ops = {
    .write = tmpfs_write,
    .read = tmpfs_read
};


int tmpfs_setup(struct filesystem* fs, struct mount* mount)
{
    // migrate root from previous filesystem to tmpfs
    mount->root->mount = mount;
    mount->root->v_ops = &tmpfs_v_ops;
    mount->root->f_ops = &tmpfs_f_ops;

    // root internal?
    return 0;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
    return 0;
}


int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
    return 0;
}


int tmpfs_write (struct file* file, const void* buf, size_t len)
{
    return 0;
}

int tmpfs_read (struct file* file, void* buf, size_t len)
{
    return 0;
}