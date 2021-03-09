#include "include/mini_uart.h"

#define CMD_SIZE 64

int BSSTEST = 0;
char *cmd_lst[][2] = {
    { "help", "list all commands"},
    { "hello", "print hello world"},
    { "reboot", "reboot"},
    { "eocl", "end of cmd list"}
};

void memzero(char *bss_begin, unsigned long len)
{
    char *mem_ptr = bss_begin;
    for (int i = 0; i < len; ++i)
        *mem_ptr++ = 0;
    return;
}

int strlen(char *str)
{
    int cnt;
    char *str_ptr;

    cnt = 0;
    str_ptr = str;
    while (*str_ptr++ != '\0')
        cnt++;
    return cnt;
}

int strcmp(char *str1, char *str2)
{
    int i;

    i = 0;
    while (str1[i] != '\0') {
        if (str1[i] != str2[i])
            return 1;
        i++;
    }
    if (str2[i] != '\0')
        return 1;
    return 0;
}

int do_help(void)
{
    for (int i = 0; strcmp(cmd_lst[i][0], "eocl"); ++i) {
        uart_send_string(cmd_lst[i][0]);
        uart_send_string(" : ");
        uart_send_string(cmd_lst[i][1]);
        uart_send_string("\r\n");
    }
    return 0;
}

int do_hello(void)
{
    uart_send_string("Hello World!\r\n");
    return 0;
}

int do_reboot(void)
{
    uart_send_string("reboot\r\n");
    return 0;
}

int cmd_handler(char *cmd)
{
    if (!strcmp(cmd, "help"))
        return do_help();
    if (!strcmp(cmd, "hello"))
        return do_hello();
    if (!strcmp(cmd, "reboot"))
        return do_reboot();

    uart_send_string("Command '");
    uart_send_string(cmd);
    uart_send_string("' not found\r\n");
    return 0;
}

int kernel_main(int ac, char const *av[])
{
    char ch, cmd_buf[CMD_SIZE], *cmd_ptr;

    uart_init();
    uart_send_string("Welcome to RPI3-OS\n");
    while (1) {
        uart_send_string("user@rpi3:~$ ");
        cmd_ptr = cmd_buf;
        while ((ch = uart_recv()) != '\r') {
            uart_send(ch);
            *cmd_ptr++ = ch;
        }
        uart_send_string("\r\n");

        *cmd_ptr = '\0';
        if (!strlen(cmd_buf))  // User just input Enter
            continue;
        cmd_handler(cmd_buf);
    }
    return 0;
}