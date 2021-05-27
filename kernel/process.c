#include "../lib/uart.h"
#include "process.h"
#include "reader.h"
#include "thread.h"

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