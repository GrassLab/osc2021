#include "vfs.h"
#include "dynamic_alloc.h"
#include "tmpfs.h"

struct mount* rootfs;
extern struct filesystem tmpfs;

void init_root()
{
    // prepare the root mount struct
    rootfs = malloc(sizeof(struct mount));
    rootfs->fs = &tmpfs;
    struct vnode *root_vnode = malloc(sizeof(struct vnode));
    rootfs->root = root_vnode;

    tmpfs_setup(&tmpfs, rootfs);
}

int register_filesystem(struct filesystem* fs) {
    // register the file system to the kernel.
    
    return 0;
}

struct file* vfs_open(const char* pathname, int flags) {
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.

    return NULL;
}
int vfs_close(struct file* file) {
    // 1. release the file descriptor

    return 0;
}
int vfs_write(struct file* file, const void* buf, size_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.

    return 0;
}
int vfs_read(struct file* file, void* buf, size_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.

    return 0;
}
