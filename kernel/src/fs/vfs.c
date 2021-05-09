#include "vfs.h"
#include "dynamic_alloc.h"
#include "tmpfs.h"
#include "string.h"
#include "io.h"

struct mount* rootfs;

struct filesystem_list fs_list = {
    .count = 0,
    .head = NULL,
    .tail = NULL
};

void init_root(const char *name)
{
    // get fs from pool
    struct filesystem *fs = get_filesystem(name);

    if (fs) {
        // prepare the root mount struct
        rootfs = malloc(sizeof(struct mount));
        rootfs->fs = fs;
        struct vnode *root_vnode = malloc(sizeof(struct vnode));
        rootfs->root = root_vnode;
        tmpfs_setup(fs, rootfs);
    } else {
        printf("no such filesystem: %s\n", name);
    }
}

int register_filesystem(struct filesystem* fs) {
    // create a file system node
    struct filesystem_node *fs_node = malloc(sizeof(struct filesystem_node));
    fs_node->fs = fs;
    fs_node->next_node = NULL;

    // register the file system to the kernel.
    if (fs_list.count == 0) {
        fs_list.head = fs_node;
        fs_list.tail = fs_node;
    } else {
        fs_list.tail->next_node = fs_node;
        fs_list.tail = fs_node;
    }

    fs_list.count++;
    
    return 0;
}

// get filesystem ptr by name from fs list
struct filesystem *get_filesystem(const char *fs_name) {
    struct filesystem_node *tmp_node;
    if (fs_list.count > 0) {
        tmp_node = fs_list.head;
        
        while(tmp_node) {
            if (strcmp(tmp_node->fs->name, fs_name) == 0) {
                return tmp_node->fs;
            }

            tmp_node = tmp_node->next_node;
        }

    }

    return NULL;
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
