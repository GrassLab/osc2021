#include "syscall.h"

int main(int args, char** argv) {
    int a = open("hello", O_CREATE);
    int b = open("world", O_CREATE);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("hello", 0);
    a = open("world", 0);

    char buf[100];
    int sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_printf("%s\n", buf);
    exit();

    return 0;
}