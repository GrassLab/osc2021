#include "shell.h"
#include "uart.h"
#include "gpio.h"
#include "bootloader.h"
#include "string.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC     ((volatile unsigned int*)(MMIO_BASE + 0x0010001c))
#define PM_WDOG     ((volatile unsigned int*)(MMIO_BASE + 0x00100024))

#define BUFFER_SIZE         50
#define COMMAND_LIST_LEN    (sizeof(command_list) / sizeof(struct Command))

struct Command
{
    char *name;
    void (*func)();
    char *description;
} static const command_list[] = {
    {.name = "help", .func = &help, .description = "print all available commands"},
    {.name = "hello", .func = &hello, .description = "print Hello World!"},
    {.name = "loadimg", .func = &loadimg, .description = "load kernel from uart"},
    {.name = "reboot", .func = &reset, .description = "reboot"},
};

void loadimg()
{
    bootloader();
    return;
}

void help()
{
    uart_put_str("command list :\n");
    for(int i = 0; i < COMMAND_LIST_LEN; i++)
    {
        uart_put_str(command_list[i].name);
        uart_put_str("\t: ");
        uart_put_str(command_list[i].description);
        uart_put_str("\n");
    }
    return;
}

void hello()
{
    uart_put_str("Hello World!\n");
    return;
}


void reset()
{
    uart_put_str("reboot...\n");
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | 1000;
    return;
}

// return char *args and *command is command's name
char *split_command_args(char *command)
{
    char *str;
    while(*command != ' ')
    {
        if(*command == '\0')
            return "";
        command++;
    }
    // ptr -> ' '
    *command = '\0';

    while(*command++ == ' ');
    if(*command == '\0')
        return "";
    str = command;
    
    while(*command++ != '\0');
    command--;
    // ptr -> '\0'
    while(*--command == ' ')
        *command = '\0';
    
    return str;
}

void do_command(char *command)
{
    //char *command_args = split_command_args(command);
    for(int i = 0; i < COMMAND_LIST_LEN; i++)
    {
        if(strcmp(command, command_list[i].name))
        {
            command_list[i].func();
            return;
        }
    }

    if(strcmp(command, ""))
    {
        return;
    }
    else
    {
        uart_put_str("command not found.\n");
        return;
    }
}

void get_command()
{
    char command_buffer[50] = "";
    int command_counter = 0;
    while(1)
    {
        command_buffer[command_counter++] = uart_get_char();
        uart_put_str(&command_buffer[command_counter - 1]);
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
            uart_put_str("Buffer is full.\n");
            return;
        }
    }
}

void shell()
{
    while(1)
    {
        uart_put_str("pi@bootloader ~> ");
        get_command();
    }
}