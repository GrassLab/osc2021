#include "inc/syscall.h"

int main(int argc, char **argv){
    uart_printf("Argv Test, pid %d\n", getpid());
    for (int i = 0; i < argc; ++i) {
        uart_printf("%s\n",argv[i]);
    }
    char *fork_argv[] = {"fork_test", 0};
    exec("app22", fork_argv);
}
