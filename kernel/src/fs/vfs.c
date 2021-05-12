#include "vfs.h"
#include "dynamic_alloc.h"
#include "tmpfs.h"
#include "string.h"
#include "io.h"
#include "cpio.h"
#include "thread.h"
#include "system_call.h"


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
        rootfs->fs->setup_mount(fs, rootfs);
        cpio_init_fs(root_vnode);
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

    struct file *f = NULL;
    struct vnode *target;
    int res = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

    if (flags == O_CREAT || res == 0) {

        // deal with create
        if (res != 0 && rootfs->root->v_ops->create(rootfs->root, &target, pathname) != 0) {
            // no parent dir
            return f;
        }

        // initialize file
        f = malloc(sizeof(struct file));
        f->vnode = target;
        f->f_ops = target->f_ops;
        f->f_pos = 0;
        f->flags = flags;
    }
    


    return f;
}
int vfs_close(struct file* file) {
    // 1. release the file descriptors
    free(file);

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
    return file->f_ops->read(file, buf, len);
}


// open system call handler
void do_open(const char* pathname, int flags)
{
    struct file *f = vfs_open(pathname, flags);
    // insert into thread's fd table
    struct Thread *thread = get_current_thread();
    int fd = insert_fd(&thread->fd_table, f);

    // return to user space
    struct context *ctx = (struct context *)thread->kernel_sp;
    ctx->reg[0] = fd;
}

int open(const char* pathname, int flags)
{
    return sys_open(pathname, flags);
}