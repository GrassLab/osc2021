#include "stdlib.h"
#include "syscall.h"

int getpid() {
    asm(
        "mov x8, " STR(SYS_GETPID) "\n\t"
        "svc #0\n\t"
        "ret"
    );
}

size_t read(const char *buf, size_t size) {
    asm(
        "mov x8, " STR(SYS_READ) "\n\t"
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        "svc #0\n\t"
        "ret"
    :"r"(buf),"r"(size)::"memory");
}


size_t write(const char *buf, size_t size) {
    asm(
        "mov x8, " STR(SYS_WRITE) "\n\t"
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        "svc #0\n\t"
        "ret"
    :"r"(buf),"r"(size));
}


int exec(const char *path, char *const argv[]) {
    asm(
        "mov x8, " STR(SYS_READ) "\n\t"
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        "svc #0\n\t"
        "ret"
    :"r"(path),"r"(argv));
}