#include "lib.h"

int main (int argc, char *argv[]) {
    printf("Argv Test, pid %d\n", get_pid());
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    printf("\n");
    char *fork_argv[] = {"fork_test", 0};
    exec("fork_test", fork_argv);

    return 0;
}
