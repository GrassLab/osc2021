#include <stdlib.h>
#include <printf.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        return -1;
    }

    int ret;
    char buf[0x30];
    struct file_stat stat;
    stat.name = buf;

    int fd = open(argv[1], O_DIRECTORY);
    if (fd < 0) {
        printf("directory '%s' not found.\r\n", argv[1]);
        return -1;
    }

    while (1) {
        ret = read(fd, &stat, sizeof(buf));
        if (ret <= 0) {
            break;
        }
        printf("%crwxrwxrwx root root %6lu %s\r\n", S_ISDIR(stat.f_mode) ? 'd' : '-', stat.size, stat.name);
    }

    write(fd, NULL, 0);
    close(fd);

    return 0;
}