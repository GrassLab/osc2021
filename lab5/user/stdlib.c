#include <stdlib.h>
#include <syscall.h>

int getpid() {
    int ret;
    asm(
        "mov x8, #" STR(SYS_GETPID) "\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret)::"x0","x8");
    return ret;
}

size_t uart_read(const char *buf, size_t size) {
    size_t ret;
    asm(
        "mov x8, #" STR(SYS_UART_READ) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(buf),"r"(size):"x0", "x1", "x8", "memory");

    return ret;
}


size_t uart_write(const char *buf, size_t size) {
    size_t ret;
    asm(
        "mov x8, #" STR(SYS_UART_WRITE) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(buf),"r"(size):"x0", "x1", "x8");

    return ret;
}


int exec(const char *path, char *const argv[]) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_EXEC) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(path),"r"(argv):"x0", "x1", "x8");

    return ret;
}

int fork() {
    int ret;
    asm(
        "mov x8, #" STR(SYS_FORK) "\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret)::"x0", "x8");

    return ret;
}

void exit(int status) {
    asm(
        "mov x8, #" STR(SYS_EXIT) "\n\t"
        "mov x0, %0\n\t"
        "svc #0"
    ::"r"(status));
}