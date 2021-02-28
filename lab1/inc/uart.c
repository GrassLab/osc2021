#include "uart.h"

//GPIO registers
#define MMIO_BASE 0x3F000000
#define GPFSEL1         ((volatile unsigned int*)(MMIO_BASE+0x00200004))
#define GPPUD           ((volatile unsigned int*)(MMIO_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(MMIO_BASE+0x00200098))

//auxilary mini UART registers
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))

void uart_init(){
	register unsigned int r;

	//init UART
	*AUX_ENABLE|=1;//enable mini UART
	*AUX_MU_CNTL=0;//disable transmitter and receiver during configuration
	*AUX_MU_IER=0;//disable interrupt because currently you don’t need interrupt
	*AUX_MU_LCR=3;//set the data size to 8 bit
	*AUX_MU_MCR=0;//don’t need auto flow control
	*AUX_MU_BAUD=270;//set baud rate to 115200
	*AUX_MU_IIR=0xc6;//no FIFO

	//change alternate function
	r=*GPFSEL1;
	r&=~((7<<12)|(7<<15));
	r|=(2<<12)|(2<<15);//alt5
	*GPFSEL1=r;

	//disable GPIO pull up/down
	*GPPUD=0;
	r=150;while(r--){ asm volatile("nop"); }
	*GPPUDCLK0=(1<<14)|(1<<15);
	r=150;while(r--){ asm volatile("nop"); }
	*GPPUDCLK0=0;

	*AUX_MU_CNTL=3;//enable the transmitter and receiver
}

void uart_send(unsigned int c){
	do{ asm volatile("nop"); }while(!(*AUX_MU_LSR&0x20));
	*AUX_MU_IO=c;
}

char uart_getc(){
	char r;
	do{ asm volatile("nop"); }while(!(*AUX_MU_LSR&0x01));
	r=(char)(*AUX_MU_IO);
	return r=='\r'?'\n':r;
}

void uart_puts(char *s){
	while(*s){
		uart_send(*s);
		s++;
	}
}

int strcmp(char* a,char* b){
	while(*a){
		if(*a!=*b)return 1;
		a++;
		b++;
	}
	if(*a!=*b)return 1;
	return 0;
}