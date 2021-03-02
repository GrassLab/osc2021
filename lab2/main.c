#include "inc/uart.h"
#include "inc/reboot.h"
#include "inc/mailbox.h"

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
			reboot();
		}else{
			uart_puts("Error: No such command \"");
			uart_puts(buffer);
			uart_puts("\".\n");
		}
	}
}

void main(){
	uart_init();

	unsigned int v[2];
	if(getBoardRevision(v)){
		uart_printf("board revision: 0x%x\n",v[0]);
	}else{
		uart_printf("fail: getBoardRevision\n");
	}
	if(getVCMEM(v)){
		uart_printf("VC Core base address: 0x%x\n",v[0]);
		uart_printf("VC memory size: %d\n",v[1]);
	}else{
		uart_printf("fail: getVCMEM\n");
	}

	shell();
}