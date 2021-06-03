#include "utils.h"
#include "systemcall.h"

int main(int argc, char **argv) {
    char str[15];
    int pid = getpid();
    int2str(pid, str, 15);
    uart_write("Argv Test, pid ", 15);
    uart_write(str, int_len(pid));
    uart_write("\n", 1);

    for (int i = 0; i < argc; ++i) {
        int len = 0;
        while (argv[i][len++]);
        uart_write(argv[i], len);
        uart_write("\n", 1);
    }
    char *fork_argv[2];
    fork_argv[0] = "fork_test";
    fork_argv[1] = 0;
    exec("fork_test", fork_argv);
    return 0;
}