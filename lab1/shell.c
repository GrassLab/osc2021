#include "shell.h"
#include "uart.h"
#include "gpio.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     ((volatile unsigned int*)(MMIO_BASE + 0x0010001c))
#define PM_WDOG     ((volatile unsigned int*)(MMIO_BASE + 0x00100024))

void help()
{
    uart_puts("command list :\n");
    uart_puts("help\t: print all available commands\n");
    uart_puts("hello\t: print Hello World!\n");
    uart_puts("reboot\t: reboot\n");
    return;
}

void hello()
{
    uart_puts("Hello World!\n");
    return;
}


void reset()
{
    uart_puts("reboot...\n");
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 1000;
    return;
}

int compare_str(char *s1, char *s2)
{
    while(*s1++ == *s2++)
    {
        if(*(s1-1) == '\0' || *(s2-1) == '\0')
        {
            break;
        }
    }
    if(*(s1-1) == '\0' && *(s2-1) == '\0')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void do_command(char *command)
{
    if(compare_str(command, "help"))
    {
        help();
        return;
    }
    else if(compare_str(command, "hello"))
    {
        hello();
        return;
    }
    else if(compare_str(command, "reboot"))
    {
        reset();
        return;
    }
    else if(compare_str(command, ""))
    {
        return;
    }
    else
    {
        uart_puts("command not found.\n");
        return;
    }
}

void get_command()
{
    char command_buffer[50] = "";
    int command_counter = 0;
    while(1)
    {
        command_buffer[command_counter++] = uart_getc();
        uart_puts(&command_buffer[command_counter - 1]);
        if(command_buffer[command_counter - 1] == '\n')
        {
            
            command_buffer[command_counter - 1] = '\0';
            do_command(command_buffer);
            return;
        }
        if(command_counter > 49)
        {
            for(int i = 0 ; i < command_counter ; i++) {
                command_buffer[i] = '\0';
            }
            uart_puts("Buffer is full.\n");
            return;
        }
        
    }
}

void shell()
{
    while(1)
    {
        uart_puts("pi@rpi3 ~> ");
        get_command();
    }
}