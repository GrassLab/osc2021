#include <stdlib.h>
#include <printf.h>

int main(int argc, char **argv) {
    printf("[Init] pid %d\r\n", getpid());
    for (int i = 0; i < argc; ++i) {
        printf("#%d: %s\r\n", i, argv[i]);
    }
    char *args[] = {"/bin/sh", NULL};
    exec("/bin/sh", args);

    return 0;
}