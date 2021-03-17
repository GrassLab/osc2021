#ifndef UART_H
#define UART_H

void uart_init();
void uart_sendchar(unsigned int c);
char uart_getchar();
void uart_putstr(char *str);

#endif