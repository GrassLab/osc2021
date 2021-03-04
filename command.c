#include "command.h"
#include "uart.h"
#include "string.h"

void InputBufferOverflowMessage(char cmd[])
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.");
}

void CommandHelp()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint this help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\"\n");
    uart_puts("\ttimestamp:\tget current timestamp.\n");
    uart_puts("\n");
}

void CommandHello()
{
    uart_puts("Hello World!\n");
}

void CommandTimestamp()
{
    unsigned long int cnt_freq, cnt_tpct;
    char str[20];

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
	"mrs %1, cntpct_el0 \n\t"
	: "=r" (cnt_freq), "=r" (cnt_tpct)
	:
    );

    ftoa((float)cnt_tpct / cnt_freq, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
} 

void CommandNotFound(char *s)
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void CommandReboot()
{
    uart_puts("Start Rebooting...\n");

    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 100;

    while(1);
}
