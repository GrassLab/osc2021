#ifndef __UART__
#define __UART__

#include "dtb.h"

void uart_init();
char uart_get_char();
char *uart_get_str();
int uart_get_int();
void uart_send(unsigned int);
void uart_put_str(char *);
void uart_put_int(int num);
void uart_put_ulong(unsigned long num);
void uart_put_addr(unsigned long addr);
int uart_probe(fdt_header *header, unsigned long node_addr, int depth);

#endif