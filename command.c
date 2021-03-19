#include "uart.h"
#include "string.h"

void input_buffer_overflow_message ( char cmd[] )
{
    uart_puts("Follow command: \"");
    uart_puts(cmd);
    uart_puts("\"... is too long to process.\n");

    uart_puts("The maximum length of input is 64.");
}

void command_help ()
{
    uart_puts("\n");
    uart_puts("Valid Command:\n");
    uart_puts("\thelp:\t\tprint help.\n");
    uart_puts("\thello:\t\tprint \"Hello World!\".\n");
	uart_puts("\treboot:\t\treboot.\n");
	uart_puts("\tcancel:\t\tcancel reboot.\n");
    uart_puts("\n");
}

void command_hello ()
{
    uart_puts("Hello World!!\n");
}


void command_not_found (char * s) 
{
    uart_puts("Err: command ");
	uart_puts(s);
    uart_puts(" not found, try <help>\n");
}


void reset(){ // reboot after watchdog timer expire
  uart_puts("Start Rebooting...\n");
  *PM_RSTC = PM_PASSWORD | 0x20; // full reset
  *PM_WDOG = PM_PASSWORD | 100; // number of watchdog tick
}

void cancel_reset() {
  uart_puts("Cancel Rebooting...\n");
  *PM_RSTC = PM_PASSWORD | 0; // full reset
  *PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}