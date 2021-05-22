#include <stdlib.h>
#include <printf.h>

int main(int argc, char **argv) {
    printf("Argv Test, pid %d\n", getpid());
    for (int i = 0; i < argc; ++i) {
        printf("#%d: %s\n", i, argv[i]);
    }
    char *args[] = {"bin/sh", NULL};
    exec("bin/sh", args);

    return 0;
}