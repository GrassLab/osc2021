#include <fs/vfs.h>
#include <file.h>
#include <current.h>
#include <fs/fs_struct.h>
#include <preempt.h>
#include <mm.h>
#include <syscall_wrapper.h>

static inline int pop_unused_fd(unsigned int *n) {
    int idx = __builtin_ctz(*n);
    *n ^= (1 << idx);

    return idx;
}

int do_open(const char *pathname, int flags) {
    struct task_struct *ts = current;
    struct file *f = vfs_open(pathname, flags);
    int fd;

    if (!f) {
        return -1;
    }

    disable_preempt();

    if (!ts->files) {
        ts->files = kmalloc(sizeof(struct files_struct));
        ts->files->unused_bits = -1;
    }

    /* kernel only support 32 fds per thread */
    if (!ts->files->unused_bits) {
        fd = -2;
        goto failed;
    }

    fd = pop_unused_fd(&ts->files->unused_bits);
    ts->files->fd_array[fd] = f;

failed:
    enable_preempt();

    return fd;
}

ssize_t do_read(int fd, void *buf, size_t count) {
    if (fd < 0 || fd >= NR_OPEN_DEFAULT) {
        return -1;
    }

    disable_preempt();

    struct task_struct *ts = current;
    ssize_t ret;

    /* CHECK: or should we allocate every time ? */
    if (!ts->files) {
        ret = -1;
        goto failed;
    }

    if (ts->files->unused_bits & (1 << fd)) {
        ret = -1;
        goto failed;
    }

    ret = vfs_read(ts->files->fd_array[fd], buf, count);

failed:
    enable_preempt();
    return ret;
}

ssize_t do_write(int fd, const void *buf, size_t count) {
    if (fd < 0 || fd >= NR_OPEN_DEFAULT) {
        return -1;
    }

    disable_preempt();

    struct task_struct *ts = current;
    ssize_t ret;

    /* CHECK: or should we allocate every time ? */
    if (!ts->files) {
        ret = -1;
        goto failed;
    }

    if (ts->files->unused_bits & (1 << fd)) {
        ret = -1;
        goto failed;
    }

    ret = vfs_write(ts->files->fd_array[fd], buf, count);

failed:
    enable_preempt();
    return ret;
}

int do_close(int fd) {
    if (fd < 0 || fd >= NR_OPEN_DEFAULT) {
        return -1;
    }

    disable_preempt();

    struct task_struct *ts = current;
    int ret;

    /* CHECK: or should we allocate every time ? */
    if (!ts->files) {
        ret = -1;
        goto failed;
    }

    if (ts->files->unused_bits & (1 << fd)) {
        ret = -1;
        goto failed;
    }

    ret = vfs_close(ts->files->fd_array[fd]);
    ts->files->unused_bits ^= (1 << fd);

failed:
    enable_preempt();
    return ret;
}

SYSCALL_DEFINE2(open, const char *, pathname, int, flags) {
    return do_open(pathname, flags);
}

SYSCALL_DEFINE3(read, int, fd, void *, buf, size_t, count) {
    return do_read(fd, buf, count);
}

SYSCALL_DEFINE3(write, int, fd, const void *, buf, size_t, count) {
    return do_write(fd, buf, count);
}

SYSCALL_DEFINE1(close, int, fd) {
    return do_close(fd);
}

SYSCALL_DEFINE1(fsync, int, fd) {
    if (fd < 0 || fd >= NR_OPEN_DEFAULT) {
        return -1;
    }

    disable_preempt();

    struct task_struct *ts = current;
    int ret;

    /* CHECK: or should we allocate every time ? */
    if (!ts->files) {
        ret = -1;
        goto failed;
    }

    if (ts->files->unused_bits & (1 << fd)) {
        ret = -1;
        goto failed;
    }

    ret = vfs_fsync(ts->files->fd_array[fd]);

failed:
    enable_preempt();
    return ret;
}