#include "process.h"
#include "reader.h"
#include "thread.h"

int do_getpid()
{

}

size_t do_uart_read(char buffer, size_t size)
{

}

size_t do_uart_write(const char buffer[], size_t size)
{

}

int do_fork()
{
    
}

int do_exec(const char * name, const char * argv[])
{
    cpio_exec(name, argv);
}

int do_exit()
{
    // thread exit
    exit();
}