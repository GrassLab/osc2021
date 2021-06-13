#include <stdlib.h>
#include <syscall.h>
typedef long ssize_t;

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

int open(const char *pathname, int flags) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_OPEN) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(pathname), "r"(flags):"x0", "x1", "x8");

    return ret;
}

ssize_t read(int fd, void *buf, size_t count) {
    ssize_t ret;
    asm(
        "mov x8, #" STR(SYS_READ) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "mov x2, %3\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(fd), "r"(buf), "r"(count):"x0", "x1", "x2", "x8", "memory");

    return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
    ssize_t ret;
    asm(
        "mov x8, #" STR(SYS_WRITE) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "mov x2, %3\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(fd), "r"(buf), "r"(count):"x0", "x1", "x2", "x8");

    return ret;
}

int close(int fd) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_CLOSE) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(fd):"x0", "x1", "x8");

    return ret;
}

int getcwd(char *buf, size_t size) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_GETCWD) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(buf), "r"(size):"x0", "x1", "x8");

    return ret;
}

int mkdir(const char *path) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_MKDIR) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(path):"x0", "x1", "x8");

    return ret;
}

int rmdir(const char *path) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_RMDIR) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(path):"x0", "x1", "x8");

    return ret;
}

int chdir(const char *path) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_CHDIR) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(path):"x0", "x1", "x8");

    return ret;
}

int mount(const char *source, const char *target, const char *fs_name) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_MOUNT) "\n\t"
        "mov x0, %1\n\t"
        "mov x1, %2\n\t"
        "mov x2, %3\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(source), "r"(target), "r"(fs_name):"x0", "x1", "x2", "x8");

    return ret;
}

int umount(const char *target) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_UMOUNT) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(target):"x0", "x1", "x8");

    return ret;
}

int sleep(size_t msec) {
    int ret;
    asm(
        "mov x8, #" STR(SYS_SLEEP) "\n\t"
        "mov x0, %1\n\t"
        "svc #0\n\t"
        "mov %0, x0"
    :"=r"(ret):"r"(msec):"x0", "x1", "x8");

    return ret;
}