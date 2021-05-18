#include "../../lib/uart.h"
#include "../../lib/sys_call.h"

int main()
{
    uart_puts("Fork Test, pid ");
    uart_puts_i(getpid());
    
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

            delay(1000000);
            ++cnt;
        }
    }
    else
    {
        uart_puts("parent here, pid: ");
        uart_puts_i(getpid());
        uart_puts(", child: ");
        uart_puts_i(ret);
    }
}