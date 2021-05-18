#include <io.h>

int main(int argc, char ** argv) {
    if (argc != 3) {
        io() << "Argument not met\r\n";
        return -1;
    }
    if (strlen(argv[1]) > 10) {
        io() << "Filename too long\r\n";
        return -1;
    }
    int fd = open(argv[1], O_WRONLY | O_CREAT);
    if (fd < 0) {
        io() << "Failed to create file \"" << argv[1] << "\".\r\n";
        return -1;
    }
    write(fd, argv[2], strlen(argv[2]));
    close(fd);
    return 0;
}
