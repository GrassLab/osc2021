#include "utils.h"
void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void loadimg();
void uart_int(unsigned long s);
unsigned int uart_printf(char* fmt,...);
unsigned char uart_getb();
unsigned long uart_getX(int display);
unsigned long uart_getU(int display);
int uart_gets(char* s,int size,int display);
int uart_writes(char* s,int size);
int strcmp(const char* a,const char* b);