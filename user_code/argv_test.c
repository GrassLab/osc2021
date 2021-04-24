#include "lib/printf.h"
#include "lib/system_call.h"
int main(int argc, char **argv) {
    printf("Argv Test, pid %d\n", getpid());
    for (int i = 0; i < argc; ++i) {
        printf("%s",argv[i]);
    }
    char *fork_argv[] = {"fork_test", 0};
    exec("fork_test", fork_argv);
}