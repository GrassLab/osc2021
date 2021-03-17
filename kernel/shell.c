#include "shell.h"
#include "string.h"
#include "uart.h"
#include "cpio.h"

void shell_welcome_message() {
    uart_puts("\n");
    uart_puts(" _   _      _ _       \n");
    uart_puts("| | | |    | | |      \n");
    uart_puts("| |_| | ___| | | ___  \n");
    uart_puts("|  _  |/ _ \\ | |/ _ \\ \n");
    uart_puts("| | | |  __/ | | (_) |\n");
    uart_puts("\\_| |_/\\___|_|_|\\___/ \n");
    uart_puts("\n");
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
    else if (!strcmp("ls"              , cmd))     { command_ls(); }
    //else if (!strcmp("read"            , cmd))     { command_read_file_from_index(); }
    else    { command_not_found(); }
}

void command_help() {
    uart_puts("========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("  help\t:\tprint all available commands.\n");
    uart_puts("  hello\t:\tprint Hello World!\n");
    uart_puts("  reboot:\treboot ths system.\n");
    uart_puts("  cpio_info:\tshow cpio info.\n");
    uart_puts("  ls:\tshow cpio list.\n");
    uart_puts("  read:\tread file from index.\n");
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
    struct cpio_info info;

    if(cpio_info((void *)CPIO_ARCHIVE_LOCATION, &info)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    char output_buffer[30];

    uart_puts("file count: ");
    itoa(info.file_count, output_buffer, 10);
    uart_puts(output_buffer);
    uart_puts("\n");

    uart_puts("maximum size of a file name: ");
    itoa(info.max_path_sz, output_buffer, 10);
    uart_puts(output_buffer);
    uart_puts("\n");
}

void command_ls() {
    struct cpio_info info;

    if(cpio_info((void *)CPIO_ARCHIVE_LOCATION, &info)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    char ls_buffer[100][100];
    unsigned long buf_len = 100;

    for(int i = 0; i < 100; i++)
        for(int j = 0; j < 100; j++)
            ls_buffer[i][j] = 0;

    cpio_ls((void *)CPIO_ARCHIVE_LOCATION, ls_buffer, buf_len);

    for(int i = 0; i < info.file_count; i++) {
        char output_buffer[10] = {0};
        itoa(i, output_buffer, 10);
        uart_puts(output_buffer);
        uart_puts(": ");
        uart_puts(ls_buffer[i]);
        uart_puts("\n");
    }
}

/*
void command_read_file_from_index() {
    struct cpio_info info;

    if(cpio_info((void *)CPIO_ARCHIVE_LOCATION, &info)) {
        uart_puts("Error on cpio_info command\n");
        return;
    }

    uart_puts("index: ");
    char input_c = uart_getc();

    uart_send(input_c); 
    int index = (int)input_c - 48;
    uart_puts("\n");


    char name_buffer[100][100];
    unsigned long size = 0;

    for(int i = 0; i < 100; i++)
        for(int j = 0; j < 100; j++)
            name_buffer[i][j] = 0;

    void *fd = cpio_get_entry((void *)CPIO_ARCHIVE_LOCATION, index, name_buffer, &size);

    uart_puts("name: ");
    uart_puts(name_buffer[index]);
    uart_puts("\n");

    char output_buffer[100] = {0};
    itoa(size, output_buffer, 10);
    uart_puts("size: ");
    uart_puts(output_buffer);
    uart_puts("\n");

    for(int i = 0; i < 100; i++) {
        output_buffer[i] = 0;
    }

    for(int i = 0; i < size; i++) {
        output_buffer[i] = *(char *)((fd + (122*8) + i));
    }

    uart_puts("content: ");
    uart_puts(output_buffer);
    uart_puts("\n");

}
*/

void command_not_found() {
    uart_puts("command not found\n");
}