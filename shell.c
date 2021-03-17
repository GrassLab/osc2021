#include "shell.h"
#include "string.h"
#include "uart.h"

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
			command(buffer);
			break;
		} else {
			buffer[i] = c;
			i++;
		}
	}
}

void reboot() {
	uart_puts("Start Rebooting...\n");

	*PM_WDOG = PM_PASSWORD | 0x20;
	*PM_RSTC = PM_PASSWORD | 100;
	
	while(1);
}

void command(char *cmd) {
	if(!strcmp("help", cmd)) {
		uart_puts("========================================\n");
		uart_puts("Usage\t:\t<command> \n");
		uart_puts("help\t:\tprint all available commands.\n");
		uart_puts("hello\t:\tprint Hello World!\n");
		uart_puts("reboot\t:\treboot the system.\n");
		uart_puts("========================================\n");
	} else if(!strcmp("hello", cmd)) {
		uart_puts("Hello world\n");
	} else if(!strcmp("reboot", cmd)) {
		reboot();
	} else {
		uart_puts("command not found\n");
	}
}