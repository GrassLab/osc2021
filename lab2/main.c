#include "inc/uart.h"
#include "inc/reboot.h"
#include "inc/mailbox.h"

#define min(a,b) ((a)<(b)?(a):(b))

void moveImg(){//should use register
	asm volatile("cbz x14, endmove		\n"::);
	asm volatile("ldrb w0, [x13], #1	\n"::);
	asm volatile("strb w0, [x12], #1	\n"::);
	asm volatile("sub x14, x14, #1		\n"::);
	asm volatile("br x10				\n"::);
	asm volatile("endmove:				\n"::);
	asm volatile("br x11				\n"::);
}

void loadImg(){
	extern unsigned long* __prog_start,__prog_end;
	unsigned long c_addr,c_size;
	c_addr=(unsigned long)&__prog_start;
	c_size=(unsigned long)(&__prog_end)-(unsigned long)(&__prog_start);//include stack

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
	if(c_addr>k_addr+k_size||c_addr+c_size<k_addr){//no overlape
		unsigned char* target=(unsigned char*)k_addr;
		while(k_size--){
			*target=uart_getb();
			target++;
		}

		uart_puts("loading...\n");
		asm volatile("br %0\n"::"r"(k_addr));
	}else{//overlape
		unsigned long tmp_addr=min(c_addr,k_addr)-(c_size+k_size);
		unsigned char* target=(unsigned char*)tmp_addr;
		for(int i=0;i<c_size;++i){//cur prog
			*target=((unsigned char*)c_addr)[i];
			target++;
		}
		for(int i=0;i<k_size;++i){//new prog
			*target=uart_getb();
			target++;
		}

		uart_puts("moving...\n");
		unsigned long delta=c_addr-tmp_addr;
		asm volatile("mov x10, %0	\n"::"r"((unsigned long)moveImg-delta));//copy-loop address
		asm volatile("mov x11, %0	\n"::"r"(k_addr));//load address
		asm volatile("mov x12, %0	\n"::"r"(k_addr));//copy dst
		asm volatile("mov x13, %0	\n"::"r"(tmp_addr+c_size));//copy src
		asm volatile("mov x14, %0	\n"::"r"(k_size));//copy size
		asm volatile("br x10		\n"::);
	}
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