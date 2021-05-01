#include "vt.h" 


void print_el(){
	int el = get_el();
	uart_puts("Exception Level:");
	uart_put_int(el);
	uart_puts("\n");
}

