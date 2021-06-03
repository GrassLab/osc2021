#ifndef UART
#define UART

void uart_init();
char uart_get_char();
int uart_get_int();
void uart_send(unsigned int);
void uart_put_str(char *);
void uart_put_int(int num);
void uart_put_addr(unsigned long addr);

#endif