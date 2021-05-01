#include "shell.h"


void shell_welcome(){
    uart_puts("\r\n------------------------\r\n");
    uart_puts("=        Kernel        =\r\n");
    uart_puts("------------------------\r\n");
}

void receive_cmd() {
  while (1) {
    char c = uart_async_getc();
    if (c == '\0') continue;  // to avoid weird character
    if (c == '\n') {          // '\r' is replaced with '\n'
      uart_puts("\r\n");
      //input_buffer[buffer_index] = '\0';
      break;
    }
	else if(c == '\b'){
		if(buffer_index > 0){
			uart_puts("\b \b");
			input_buffer[--buffer_index] = '\0';
		}
	}
	else{
    	uart_send(c);
    	input_buffer[buffer_index++] = c;
	}

  }
}

void clear_buffer() {
  
  buffer_index = 0;
  for (int i = 0; i < MAX_BUFFER_LEN; i++) {
    input_buffer[i] = '\0';
  }
}

void shell() {
	
#ifdef	async
	while(uart_async_getc() == '\0');
	while(uart_async_getc() == '\0');
#endif

#ifndef	async
	while(uart_get() == '\0');
	while(uart_get() == '\0');
#endif
    shell_welcome();
    uart_puts(USER_NAME);
    uart_puts("@");
    uart_puts(MACHINE_NAME);
	uart_puts("$ ");
#ifndef	async
    char input_buffer[MAX_BUFFER_LEN];
    char *buf_ptr = input_buffer;
    memset (buf_ptr, '\0', MAX_BUFFER_LEN);   
    char c;
#endif

    while(1){

#ifdef	async
		clear_buffer();
		receive_cmd();
		system_command(input_buffer);
		uart_puts(USER_NAME);
    	uart_puts("@");
    	uart_puts(MACHINE_NAME);
		uart_puts("$ ");
#endif

#ifndef	async
		c = uart_get();
		
		if(c == '\r'){
			uart_puts("\r\n");
			memset (buf_ptr, '\0', MAX_BUFFER_LEN);   
		    system_command(input_buffer);
			uart_puts(USER_NAME);
			uart_puts("@");
			uart_puts(MACHINE_NAME);
			uart_puts("$ ");
			buf_ptr = input_buffer;
			memset(buf_ptr, '\0', MAX_BUFFER_LEN);
		}
		else if(c == '\b'){
			if(buf_ptr > input_buffer){
				uart_puts("\b \b");
				*(--buf_ptr) = '\0';
			}
		}
	    else{
	        uart_send(c);
	        *buf_ptr++ = c;
        }
#endif


		
    }


}

