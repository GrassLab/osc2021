#ifndef _MINI_UART_H
#define _MINI_UART_H

void uart_init(void);
char uart_getc(void);
void uart_putc(char c);
void uart_puts(char* str);

#endif