#ifndef UART
#define UART

void uart_init();
char uart_getc();
void uart_send(unsigned int);
void uart_puts(char *);

#endif