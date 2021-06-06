#include "../lib/uart.h"
#include "../lib/string.h"
#include "process.h"
#include "reader.h"
#include "thread.h"
#include "vfs.h"
#include "fd.h"

int do_getpid()
{
    struct Thread * t = current_thread();

    return t->id;
}

size_t do_uart_read(char buffer, size_t size)
{
    // not used
    buffer = uart_getc();

    return size;
}

size_t do_uart_write(const char buffer[], size_t size)
{
    uart_puts(buffer);

    return size;
}

void do_fork(struct Trap_Frame * tf)
{
    thread_fork(tf);
}

void do_exec(const char * name, const char * argv[])
{
    cpio_exec(name, argv);
}

void do_exit()
{
    thread_exit();
}

int do_open(const char * path_name, int flags)
{
    struct File * f = vfs_open(path_name, flags);

    struct Thread * t = current_thread();

    int fd = insert_fd(t->fd_table, f);
    return fd;
}

int do_close(int fd)
{
    struct Thread * t = current_thread();
    
    vfs_close(t->fd_table[fd].f);

    remove_fd(t->fd_table, fd);

    return 0;
}

int do_write(int fd, const void * buf, size_t len)
{
    struct Thread * t = current_thread();

    struct File * f = t->fd_table[fd].f;

    int ret = vfs_write(f, buf, len);

    return ret;
}

int do_read(int fd, void * buf, size_t len)
{
    struct Thread * t = current_thread();

    struct File * f = t->fd_table[fd].f;

    int ret = vfs_read(f, buf, len);

    return ret;
}