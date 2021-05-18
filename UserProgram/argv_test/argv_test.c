#include "../../lib/uart.h"
#include "../../lib/sys_call.h"

int main(int argc, char **argv)
{
    uart_puts_h(&argc);
    uart_puts(" ");
    uart_puts_i(*(0x10009FB0));
    for (int i = 0; i < 10; ++i)
    {
        uart_puts_h(*(unsigned long*)(0x10009FB0 + i * 8));
        uart_puts(" ");
    }
    uart_puts_h(&argv);
    uart_puts("\n");

    uart_puts("Argv Test, pid: ");
    uart_puts_i(getpid());
    uart_puts("\n");

    uart_puts("argc: ");
    uart_puts_i(argc);
    uart_puts("\n");
    for (int i = 0; i < argc; ++i)
    {
        uart_puts(argv[i]);
        uart_puts("\n");
    }
    
    while(1){}

    char * fork_argv[2];
    fork_argv[0] = "fork_test";
    fork_argv[1] = 0;
    exec("fork_test.img", fork_argv);
}