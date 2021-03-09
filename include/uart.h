#include "gpio.h"
void uart_init();
void uart_send(unsigned int c);
char uart_get();
void uart_puts(char *s);
int uart_get_int();
