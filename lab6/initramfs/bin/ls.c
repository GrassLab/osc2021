#include <stdlib.h>
#include <printf.h>
#include <string.h>
#define assert(b)                                              \
    if (!(b)) {                                                \
        printf("test failed: %s, line %d\r\n", #b, __LINE__);  \
        return 0;                                              \
    }                                                          \

int main(int argc, char **argv) {
    assert(argc == 2);

    int ret;
    char buf[0x30];
    struct file_stat stat;
    stat.name = buf;

    int fd = open(argv[1], O_DIRECTORY);
    assert(fd >= 0);

    while (1) {
        ret = read(fd, &stat, sizeof(buf));
        if (ret <= 0) {
            break;
        }
        printf("%crwxrwxrwx root root %s\r\n", S_ISDIR(stat.f_mode) ? 'd' : '-', stat.name);
    }

    write(fd, NULL, 0);
    close(fd);

    return 0;
}