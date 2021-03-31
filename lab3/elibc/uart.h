#include "utils.h"
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void loadimg();
void uart_int(unsigned long s);