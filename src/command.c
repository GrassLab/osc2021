#include "uart.h"
#include "string.h"
#include "shell.h"
void input_buffer_overflow_message ( char cmd[] )
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 128.\n");
}

void command_help ()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint all available commands.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
    uart_puts("\ttimestamp:\tget current timestamp.\n");
    uart_puts("\treboot:\t\treset rpi3.\n");
    uart_puts("\n");
}

void command_hello ()
{
    uart_puts("Hello World!\n");
}

void command_timestamp ()
{
    unsigned long int cnt_freq, cnt_tpct;
    char str[20];

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r" (cnt_freq),  "=r" (cnt_tpct)
        :
    );

    ftoa( ((float)cnt_tpct) / cnt_freq, str, 6);

    uart_send('[');
    uart_puts(str);
    uart_puts("]\n");
}

void command_not_found (char * s) 
{
    uart_puts("Err: command ");
    uart_puts(s);
    uart_puts(" not found, try <help>\n");
}

void command_reboot ()
{
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}