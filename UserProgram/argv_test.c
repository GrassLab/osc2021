#include "../lib/uart.h"

int main(int argc, char **argv)
{
    uart_puts("Argv Test, pid: ");
    uart_puts_i(getpid());
    uart_puts("\n");

    for (int i = 0; i < argc; ++i)
    {
        puts(argv[i]);
    }

    char ** fork_argv[] = {"fork_test", 0};
    exec("fork_test", fork_argv);
}