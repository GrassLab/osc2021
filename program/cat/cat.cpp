#include <io.h>

int main(int argc, char ** argv) {
    if (argc != 2) {
        io() << "Argument not met\r\n";
        return -1;
    }
    if (strlen(argv[1]) > 12) {
        io() << "Filename too long\r\n";
        return -1;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        io() << "The file \"" << argv[1] << "\" does not exist.\r\n";
        return -1;
    }
    int count;
    char buffer[256];
    while ((count = read(fd, buffer, sizeof(buffer))) > 0) {
        uart_put(buffer, count);
    }
    io() << "\r\n";
    close(fd);
    return 0;
}
