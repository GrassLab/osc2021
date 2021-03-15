#include "gpio.h"
void uart_init();
void uart_send(unsigned int c);
char uart_get();
void uart_puts(char *s);
void uart_puts_bySize(char *s, int size);
int uart_get_int();
void uart_printint(unsigned long long int num);
void uart_printhex(unsigned long long int num);