#include "uart.h"
#include "string.h"
#include "shell.h"
#include "printf.h"

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
    uart_puts("\tloadKernel:\t\tload new kernel to 0x80000.\n");
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

void command_loadkernel() 
{
    int kernel_size=0;
    char buff[MAX_BUFFER_LEN];
    char *kernel=(char*)0x80000;

    printf("Loading kernel size...\n");
    // TODO: read kernel size
    int count_digit = 0;
    char char_recv;
    while (1) {
        char_recv = uart_getc();
        printf("%c", char_recv);
        
        buff[count_digit] = char_recv;

        if (char_recv == '\n') break;

        count_digit++;
    }
    buff[count_digit] = '\0'; // NULL terminator
    
    // translate kernel size variable from string to int
    kernel_size = atoi(buff);
    printf("Kernel size: %d\n", kernel_size);

    // TODO: read kernel 
    // read the kernel
    int temp = kernel_size;
    char temp_char;
    while(temp--) {
        temp_char = uart_getc();
        *kernel++ = temp_char;
    }
    printf("Jumpy to new kernel\n");
    // Note: Change to actual kernal will cause some printed character be overwrite(It still a problem to solve)
    // So we add redundant char '\n' to cover this problem
    printf("\n\n\n\n\n"); 
    
    // restore arguments and jump to the new kernel.
    asm volatile (
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret"
    );

}