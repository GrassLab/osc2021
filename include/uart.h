#ifndef UART_H
#define UART_H

void uart_init();
void uart_putchar(unsigned int c);
void uart_putstr(char* str);
char uart_getchar();
char uart_read_raw();
void uart_printf(char *fmt, ...);

#endif
