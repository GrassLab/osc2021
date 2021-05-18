#include "lib/system_call.h"

int main(int argc, char **argv){
    // uart_printf("in hello world\n");
    char buf[100];
    int a = open("hello", O_CREAT);
    int b = open("world", O_CREAT);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("hello", 0);
    a = open("world", 0);
    int sz = 0;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    uart_printf("%s\n", buf); // should be Hello World!
    exit();
    return 0;
}