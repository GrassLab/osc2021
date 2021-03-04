#include "uart.h"
#define MMIO_BASE       0x3F000000
#define PM_RSTC         ((volatile unsigned int*)(MMIO_BASE+0x0010001c))
#define PM_RSTS         ((volatile unsigned int*)(MMIO_BASE+0x00100020))
#define PM_WDOG         ((volatile unsigned int*)(MMIO_BASE+0x00100024))
#define PM_WDOG_MAGIC   0x5a000000
#define PM_RSTC_FULLRST 0x00000020

int strcmp(char *input,char *command)
{
    while(*input != '\0')
    {
        if(*input != *command)
        {
            return 0;
        }
        input = input + 1;
        command = command +1;
    }
    if (*input != *command)
    {
        return 0;
    }
    return 1;
}

void uart_read_line(char *input)
{
    int index = 0;
    char c;
    while(c != '\n')
    {
        c = uart_getc();
        uart_send(c);

        if(c != '\n')
        {
            input[index] = c;
            index =index + 1;
        }
        else
        {
            input[index] = '\0';
        }
    }

}

void hello()
{
    uart_puts("Hello World!\n");
}

void help()
{
    uart_puts("help: print all available commands\n");
    uart_puts("hello: print Hello World!\n");
}

void reboot()
{
    unsigned int r;
    r = *PM_RSTS; 
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_WDOG_MAGIC | r;  //debugger, watchdog, software
    *PM_WDOG = PM_WDOG_MAGIC | 10;  //used 20 bit count down
    *PM_RSTC = PM_WDOG_MAGIC | PM_RSTC_FULLRST;//had a watchdog full reset. clear this flag by writing 0 this field
}

void main()
{
    uart_init();
    char *welcome = "---------------------------LAB1---------------------------\n";
    uart_puts(welcome);

    while(1)
    {
        uart_puts("#");
        char input[20];
        uart_read_line(input);
        uart_puts("\r");

        if (strcmp(input, "hello"))
        {
            hello();
        }
        else if(strcmp(input,"help"))
        {
            help();
        }
        else if(strcmp(input,"reboot"))
        {
            reboot();
        }
        else
        {
            uart_puts("Error: ");
            uart_puts(input);
            uart_puts(" command not found! Try <help> to check all available commands\n");
        }
    }


}
