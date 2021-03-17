#include "shell.h"
#include "string.h"
#include "uart.h"
#include "type.h"

void shell_welcome_message() {
	uart_puts("    __  _____    ____  ______   ____  ____  ____  ________    ____  ___    ____  __________ \n");
	uart_puts("   / / / /   |  / __ \\/_  __/  / __ )/ __ \\/ __ \\/_  __/ /   / __ \\/   |  / __ \\/ ____/ __ \\\n");
	uart_puts("  / / / / /| | / /_/ / / /    / __  / / / / / / / / / / /   / / / / /| | / / / / __/ / /_/ /\n");
	uart_puts(" / /_/ / ___ |/ _, _/ / /    / /_/ / /_/ / /_/ / / / / /___/ /_/ / ___ |/ /_/ / /___/ _, _/ \n");
	uart_puts(" \\____/_/  |_/_/ |_| /_/    /_____/\\____/\\____/ /_/ /_____/\\____/_/  |_/_____/_____/_/ |_|  \n");

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
    else if (!strcmp("load_image"      , cmd))     { command_load_image(); }
    else if (!strcmp("jump_image"      , cmd))     { command_jump_to_kernel(); }
    else    { command_not_found(); }
}

void command_help() {
    uart_puts("========================================\n");
    uart_puts("Usage\t:\t<command> \n");
    uart_puts("  help\t:\tprint all available commands.\n");
    uart_puts("  hello\t:\tprint Hello World!\n");
    uart_puts("  reboot:\treboot ths system.\n");
    uart_puts("  load_image:\tload image from uart.\n");
    uart_puts("  jump_image:\tjump to 0x80000.\n");
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

void command_load_image() {
	int32_t is_receive_successful = 0;

	uart_puts("Start Loading Kernel Image...\n");
	uart_puts("Loading Kernel Image at address 0x80000...\n");
	
	char *load_address = (char *)0x80000;
	
	uart_puts("Please send image from uart now:\n");

	do {
		/* waiting 3000 cycles */
		unsigned int n = 3000;
		while ( n-- ) {
			asm volatile("nop");
		}

		/* send starting signal to receive img from host */
		uart_send(3);
		uart_send(3);
		uart_send(3);
		
		/* read kernel's size */
		int32_t size = 0;

		size  = uart_getc();
		size |= uart_getc() << 8;
		size |= uart_getc() << 16;
		size |= uart_getc() << 24;

		if (size < 64 || size > 1024*1024) {
			// size error
			uart_send('S');
			uart_send('E');            
			
			continue;
		}
		uart_send('O');
		uart_send('K');

		uart_send('\n');

		/* start receiving img */
		char *address_counter = load_address;

		while (size --) {
			*address_counter++ = uart_getc();
		}

		/* finish */
		is_receive_successful = 1;

		char output_buffer[30];

		uart_puts("Load kernel at: ");
		itohexstr((uint64_t) load_address, sizeof(char *), output_buffer);
		uart_puts(output_buffer);
		uart_send('\n');
	} while(!is_receive_successful);
}

void command_jump_to_kernel() {
	asm volatile (
		"mov x30, 0x80000;"
		"ret"
	);
}

void command_not_found() {
    uart_puts("command not found\n");
}