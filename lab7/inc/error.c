#include "error.h"
#include "uart.h"

void ERROR(char* str){
	uart_printf("%s\n",str);
	while(1){}
}