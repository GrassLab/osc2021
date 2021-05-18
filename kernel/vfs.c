#include "vfs.h"
#include "tmpfs.h"
#include "allocator.h"
#include "uart.h"

static void* write_f;
static void* read_f;
static mount mnt;

int register_filesystem(filesystem* fs) {
  // register the file system to the kernel.
    char* name=(char*)kmalloc(6);
    name[0]='t';
	name[1]='m';
	name[2]='p';
	name[3]='f';
	name[4]='s';
	name[5]='\0';
    fs->name = name;
    fs->setup_mount = tmpfs_setup(fs, &mnt);
    write_f = tmpfs_write;
    read_f = tmpfs_read;
    return 0;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
    vnode *dir = mnt.root;
    vnode *child = (vnode*)kmalloc(sizeof(vnode));
    while(1) {
        int index = tmpfs_lookup(dir, &child, pathname);
        /* if index >= 0, file already exist, else create a new file if O_CREATE*/
        if(index >= 0) {
            break;
        }
        else {
            if((flags & O_CREATE) == 0) {
                uart_puts("vfs_open(): cannot create new file!\n");
                break;
            }
            else {
                dir->v_ops->create(dir, &child, pathname);
                break;
            }
        }
    }

    file *f = (file*)kmalloc(sizeof(file));
    f->vnode = child;
    f->f_pos = 0;
    f->f_ops = (file_operations*)kmalloc(sizeof(file_operations));
    f->f_ops->read = read_f;
    f->f_ops->write = write_f;
    f->flags = flags;
    return f;
}
int vfs_close(file* file) {
  // 1. release the file descriptor
    free_page((unsigned long)file->f_ops, sizeof(file_operations));
    free_page((unsigned long)file, sizeof(file));
    return 0;
}
int vfs_write(file* file, const void* buf, unsigned long len) {
    return file->f_ops->write(file, buf, len);
}
int vfs_read(file* file, void* buf, unsigned long len) {
    return file->f_ops->read(file, buf, len);
}