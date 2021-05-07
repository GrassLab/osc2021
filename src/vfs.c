#include "vfs.h"
#include "system.h"
#include "string.h"
#include "tmpfs.h"
struct mount *rootfs;
struct filesystem tmpsfs;
struct vnode* current_dir;

void vfs_init(){
    rootfs = (struct mount *)kmalloc(sizeof(struct mount));
    tmpsfs.name = "tmpfs";
    register_filesystem(&tmpsfs);
    tmpsfs.setup_mount(&tmpsfs, rootfs);
    current_dir = rootfs->root;
}
int register_filesystem(struct filesystem *fs)
{
    // register the file system to the kernel.
    if(strcmp(fs->name, "tmpfs") == 0){
        //printf("register tmpfs\n");
        fs->setup_mount = tmpfs_setup_mount;
        tmpfs_file_operations = kmalloc(sizeof(struct file_operations));
        tmpfs_vnode_operations = kmalloc(sizeof(struct vnode_operations));
        tmpfs_file_operations->write = tmpfs_write;
        tmpfs_file_operations->read = tmpfs_read;
        tmpfs_file_operations->list = tmpfs_list;
        tmpfs_vnode_operations->lookup = tmpfs_lookup;
        tmpfs_vnode_operations->create = tmpfs_create;
        return 1;
    }
    return -1; // no legal file system
}

struct file *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    struct vnode* target;
    struct file* fd = nullptr;
    if(strcmp(pathname, "/") == 0 && flags != O_CREAT){
        target = rootfs->root;
        fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
    }
    else if(strcmp(pathname, ".") == 0 && flags != O_CREAT){
        target = current_dir;
        fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
    }
    else if(flags == O_CREAT){
        int flag = current_dir->v_ops->lookup(current_dir, &target, pathname);
        if(flag == -1){
            current_dir->v_ops->create(current_dir, &target, pathname);
            fd = (struct file*)kmalloc(sizeof(struct file));
            fd->vnode = target;
            fd->f_ops = target->f_ops;
            fd->f_pos = 0;
        }
        else{
            // exist
        }
    }
    else{
        int flag = current_dir->v_ops->lookup(current_dir, &target, pathname);
        if(flag == -1){
            // not found;
        }
        else{
            fd = (struct file*)kmalloc(sizeof(struct file));
            fd->vnode = target;
            fd->f_ops = target->f_ops;
            fd->f_pos = 0;
        }
    }
    return fd;
}
int vfs_close(struct file *file)
{
    // 1. release the file descriptor
    free(file);
    return 1;
}
int vfs_write(struct file *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    printf("hihihi\n");
    return file->f_ops->write(file, buf, len);
}
int vfs_read(struct file *file, void *buf, size_t len)
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}
int vfs_list(struct file *file, void *buf, int id){
    return file->f_ops->list(file, buf, id);
}