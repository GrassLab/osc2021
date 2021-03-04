#include "inc/uart.h"
#include "inc/reboot.h"
#include "inc/mailbox.h"

void loadImg(){
	unsigned long k_addr=0,k_size=0;
	char c;

	uart_puts("Please enter kernel load address (Hex): ");
	do{
		c=uart_getc();
		if(c>='0'&&c<='9'){
			k_addr=k_addr*16+c-'0';
		}else if(c>='a'&&c<='f'){
			k_addr=k_addr*16+c-'a'+10;
		}else if(c>='A'&&c<='F'){
			k_addr=k_addr*16+c-'A'+10;
		}
	}while(c!='\n');
	uart_printf("0x%x\n",k_addr);

	uart_puts("Please enter kernel size (Dec): ");
	do{
		c=uart_getc();
		if(c>='0'&&c<='9'){
			k_size=k_size*10+c-'0';
		}
	}while(c!='\n');
	uart_printf("%d\n",k_size);

	uart_puts("Please send kernel image now...\n");
	unsigned char* target=(unsigned char*)k_addr;
	while(k_size--){
		*target=uart_getb();
		target++;
	}

	uart_puts("loading...\n");
	((void (*)(void))k_addr)();
}

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
			uart_puts("          loadimg\n");
		}else if(strcmp(buffer,"hello")==0){
			uart_puts("Hello World!\n");
		}else if(strcmp(buffer,"reboot")==0){
			uart_puts("rebooting...\n");
			reboot();
		}else if(strcmp(buffer,"loadimg")==0){
			loadImg();
		}else{
			uart_puts("Error: No such command \"");
			uart_puts(buffer);
			uart_puts("\".\n");
		}
	}
}

void printHWInfo(){
	unsigned int v[2];

	if(getBoardRevision(v)){
		uart_printf("board revision: 0x%x\n",v[0]);
	}else{
		uart_printf("Error: getBoardRevision() fail.\n");
	}

	if(getVCMEM(v)){
		uart_printf("VC memory base address: 0x%x\n",v[0]);
		uart_printf("VC memory size: %d\n",v[1]);
	}else{
		uart_printf("Error: getVCMEM() fail.\n");
	}
}

void main(){
	uart_init();
	printHWInfo();
	shell();
}