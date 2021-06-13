#include "sys_call.h"

void dump_state()
{
    asm volatile("svc 0");
}

void exit()
{
    asm volatile("svc 1");
    while(1){}
}

int exec(const char * name, char * const argv[])
{
    long ret;
    asm volatile("svc 2");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

size_t print(const char buffer[], size_t size)
{
    long ret;
    asm volatile("svc 3");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

size_t read_buffer(char buffer[], size_t size)
{
    long ret;
    asm volatile("svc 4");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int getpid()
{
    long ret;
    asm volatile("svc 5");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int fork()
{
    long ret;
    asm volatile("svc 6");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int open(const char * path_name, int flags)
{    
    long ret;
    asm volatile("svc 7");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int close(int fd)
{
    long ret;
    asm volatile("svc 8");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int write(int fd, const void * buf, size_t len)
{
    long ret;
    asm volatile("svc 9");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}

int read(int fd, void *buf, size_t len)
{
    long ret;
    asm volatile("svc 10");
    asm volatile("mov %0, x0":"=r"(ret));
    return ret;
}