#include "gpio.h"
void uart_init();
void uart_send(unsigned int c);
char uart_get();
char* uart_gets(char*);
void uart_puts(char *s);
