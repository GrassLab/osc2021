#include "inc/uart.h"
#include "inc/reboot.h"

void shell(){
	uart_puts("Welcome!\n");
	char buffer[1000];
	int cnt;
	while(1){
		uart_puts("$ ");

		cnt=0;
		do{
			buffer[cnt++]=uart_getc();
			uart_send(buffer[cnt-1]);
		}while(buffer[cnt-1]!='\n');
		buffer[--cnt]=0;

		if(strcmp(buffer,"help")==0){
			uart_puts("commands:\n");
			uart_puts("          help\n");
			uart_puts("          hello\n");
			uart_puts("          reboot\n");
		}else if(strcmp(buffer,"hello")==0){
			uart_puts("Hello World!\n");
		}else if(strcmp(buffer,"reboot")==0){
			uart_puts("rebooting...\n");
			int tick=87;
			*PM_RSTC=PM_PASSWORD|0x20;
			*PM_WDOG=PM_PASSWORD|tick;
			while(1){
				//rebooting...
			}
		}else{
			uart_puts("Error: No such command \"");
			uart_puts(buffer);
			uart_puts("\"\n");
		}
	}
}

void main(){
	uart_init();
	
	shell();
}