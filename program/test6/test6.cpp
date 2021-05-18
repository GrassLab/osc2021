#include <io.h>

int main() {
    char buf[100];
    int a = open("hello", O_CREAT | O_WRONLY);
    int b = open("world", O_CREAT | O_WRONLY);
    write(a, "Hello ", 6);
    write(b, "World!", 6);
    close(a);
    close(b);
    b = open("hello", O_RDONLY);
    a = open("world", O_RDONLY);
    int sz;
    sz = read(b, buf, 100);
    sz += read(a, buf + sz, 100);
    buf[sz] = '\0';
    io() << buf << "\r\n"; // should be Hello World!
    return 0;
}
