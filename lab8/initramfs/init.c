#include <stdlib.h>
#include <printf.h>

int main(int argc, char **argv) {
    printf("[Init] pid %d\r\n", getpid());
    char *args[] = {"/bin/sh", NULL};
    exec("/bin/sh", args);

    return 0;
}