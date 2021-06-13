#include "../../lib/sys_call.h"
#include "../../lib/uart.h"

int main(int argc, char ** argv)
{
    int a = open("hello", O_CRET);
    int b = open("world", O_CRET);

    write(a, "Hello", 6);
    write(b, "world", 6);

    close(a);
    close(b);

    b = open("hello", 0);
    a = open("world", 0);

    int sz;
    char * buf;

    sz = read(b, buf, 100);
    sz += read(a, buf + sz - 1, 100);

    buf[sz - 1] = '\0';

    uart_puts(buf);

    exit();

    return 0;
}