#ifndef UART_H
#define UART_H

void uart_init();
void uart_putchar(unsigned int c);
void uart_putstr(char* str);
char uart_getchar();


#endif
