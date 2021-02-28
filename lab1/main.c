#include "inc/uart.h"

void shell(){
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
		}else if(strcmp(buffer,"hello")==0){
			uart_puts("Hello World!\n");
		}
	}
}

void main(){
	uart_init();
	
	shell();
}