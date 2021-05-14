#include "sd.h"
#include "allocator.h"
#include "uart.h"
#include "error.h"


void fat_Setup(){
	char* buf1=(char*)falloc(4096);
	char* buf2=(char*)falloc(4096);
	readblock(0,buf2);
	for(int i=0;i<10;++i){
		uart_printf("%d ",buf2[i]);
	}
	char v=uart_getc();
	for(int i=0;i<512;++i)buf1[i]=v;
	writeblock(0,buf1);
	ffree((unsigned long)buf1);
	ffree((unsigned long)buf2);
	uart_printf("\n");
}