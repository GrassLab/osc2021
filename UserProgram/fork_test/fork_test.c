#include "../../lib/uart.h"
#include "../../lib/string.h"
#include "../../lib/sys_call.h"

int main(int argc, char ** argv)
{
    char* temp_str = "\nFork Test, pid: ";
    print(temp_str, strlen(temp_str));
    itoa(getpid(), temp_str, 0);
    print(temp_str, strlen(temp_str));
    print("\n", 1);

    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0)
    {
        uart_puts("pid: ");
        uart_puts_i(getpid());
        uart_puts(", cnt: ");
        uart_puts_i(cnt);
        uart_puts(", ptr: ");
        uart_puts_h(&cnt);
        uart_puts("\n");

        ++cnt;
        fork();
        while (cnt < 5)
        {
            uart_puts("pid: ");
            uart_puts_i(getpid());
            uart_puts(", cnt: ");
            uart_puts_i(cnt);
            uart_puts(", ptr: ");
            uart_puts_h(&cnt);
            uart_puts("\n");

            for (int i = 0; i < 1000000; ++i)
            {
                continue;
            }

            ++cnt;
        }
    }
    else
    {
        uart_puts("**parent here, pid: ");
        uart_puts_i(getpid());
        uart_puts(", child: ");
        uart_puts_i(ret);
        uart_puts("\n\n");
    }

    exit();
    return 0;
}