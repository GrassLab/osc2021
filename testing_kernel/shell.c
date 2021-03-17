#include "shell.h"
#include "string.h"
#include "uart.h"
#include "cpio.h"

void shell_welcome_message() {
    uart_puts("┏┓┏┳━━┳┓┏┓┏━━┓\n");
    uart_puts("┃┗┛┃━━┫┃┃┃┃╭╮┃\n");
    uart_puts("┃┏┓┃━━┫┃┫┗┫╰╯┃\n");
    uart_puts("┗┛┗┻━━┻━┻━┻━━┛\n");
}

void shell_start() {
    uart_puts("$ ");

    char buffer[32];
    int i = 0;

    while(1) {
        char c = uart_getc();
        uart_send(c);
        if(c == '\n') {
            buffer[i] = 0x00;
            command_controller(buffer);
            break;
        } else {
            buffer[i] = c;
            i++;
        }
    }
}

void command_controller(char *cmd) {
    if      (!strcmp("help"            , cmd))     { command_help(); } 
    else if (!strcmp("hello"           , cmd))     { command_hello(); }
	else if (!strcmp("reboot"          , cmd))     { command_reboot(); }
    else if (!strcmp("cpio_info"       , cmd))     { command_cpio_info(); }
    else    { command_not_found(); }
}

void command_help() {
    uart_puts("========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("  help\t:\tprint all available commands.\n");
    uart_puts("  hello\t:\tprint Hello World!\n");
    uart_puts("  reboot:\treboot ths system.\n");
    uart_puts("  cpio_info:\tshow cpio info.\n");
    uart_puts("========================================\n");
}

void command_hello() {
    uart_puts("Hello world\n");
}

void command_reboot() {
    uart_puts("Start Rebooting...\n");

    *PM_WDOG = PM_PASSWORD | 0x20;
    *PM_RSTC = PM_PASSWORD | 100;
    
	while(1);
}


void command_cpio_info() {
    struct cpio_info *info_ptr;

    if(cpio_info((struct cpio_header *)CPIO_ARCHIVE_LOCATION, info_ptr)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    uart_puts("file count: ");
    uart_send(info_ptr->file_count);
    uart_puts("\n===================\n");

    uart_puts("maximum size of a file name: ");
    uart_send(info_ptr->max_path_sz);
    uart_puts("\n");

}

void command_ls() {
    uart_puts("testing...\n");
}

void command_not_found() {
    uart_puts("command not found\n");
}