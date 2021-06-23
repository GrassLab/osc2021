#include "vfs.h"
#include "tmpfs.h"
#include "fat32.h"
#include "../mm/allocator.h"
#include "../mm/cpio.h"
#include "../process/thread.h"
#include "../process/process.h"

struct mount * rootfs;
extern struct filesystem tmpfs; 
extern struct filesystem fatfs;

void init_filesystem()
{
    rootfs = (struct mount *)buddy_alloc(sizeof(struct mount));
    rootfs->root = (struct vnode *)buddy_alloc(sizeof(struct vnode));

    register_filesystem(&fatfs);
}

int register_filesystem(struct filesystem * fs)
{
    rootfs->fs = fs;
    rootfs->fs->setup_mount(fs, rootfs);
}

struct file * vfs_open(const char * path_name, int flags)
{
    struct file * f = NULL;
    struct vnode * target;
    int res = rootfs->root->v_ops->lookup(rootfs->root, &target, path_name);

    if (flags == O_CRET || res != 0)
    {
        res = rootfs->root->v_ops->create(rootfs->root, &target, path_name);
    }

    f = (struct file *)buddy_alloc(sizeof(struct file));

    f->vnode = target;
    f->f_ops = target->f_ops;
    f->f_pos = 0;
    f->flags = flags;

    return f;
}

int vfs_close(struct file * file)
{
    return 0;
}

int vfs_write(struct file * file, const void * buf, size_t len)
{
    return file->f_ops->write(file, buf, len);
}

int vfs_read(struct file * file, void * buf, size_t len)
{
    return file->f_ops->read(file, buf, len);
}

void vfs_test_1()
{
    do_exec("fs_test.img", "");
}

void vfs_test_2()
{
    int a = do_open("HELLO.TXT", 0);
    int s;
    char * buf;

    do_write(a, "Hello OSDI\n", 12);

    do_close(a);

    a = do_open("HELLO.TXT", 0);
    s = do_read(a, buf, 100);

    buf[s - 1] = '\0';

    uart_puts(buf);
}

void vfs_test(int test_id)
{
    // current default thread
    struct Thread * default_thread = thread_create(0); 
    // push temp thread to tpidr, it will save by the firset context switch
    asm volatile("msr tpidr_el1, %0"::"r"(default_thread)); 

    switch(test_id)
    {
        case 1:
            thread_create(vfs_test_1);
            idle();
        break;
        case 2:
            vfs_test_2();
        default:
        break;
    }

    return ;
}