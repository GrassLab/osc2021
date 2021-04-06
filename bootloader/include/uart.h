#ifndef _UART_H_
#define  _UART_H_

void uart_init();
char uart_getc();
void uart_putc(char);
void print(const char*);
void print_int(unsigned long long);

#endif
