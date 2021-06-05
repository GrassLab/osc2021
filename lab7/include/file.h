#ifndef __FILE_H_
#define __FILE_H_

#include <stddef.h>

#define NR_OPEN_DEFAULT 32

typedef long ssize_t;

struct files_struct {
    unsigned unused_bits;
    struct file *fd_array[NR_OPEN_DEFAULT];
};

struct file {
    struct vnode *vnode;
    size_t f_pos;
    struct file_operations *f_ops;
    unsigned int f_mode;
    int refcnt;
};

int do_open(const char *pathname, int flags);
ssize_t do_read(int fd, void *buf, size_t count);
ssize_t do_write(int fd, const void *buf, size_t count);
int do_close(int fd);

#endif