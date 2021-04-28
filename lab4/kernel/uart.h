#ifndef UART_H
#define UART_H

void uart_init(); // init
void uart_sendchar(unsigned int c); // send char
char uart_getchar(); // get char
void uart_putstr(char *str); // send string

#endif