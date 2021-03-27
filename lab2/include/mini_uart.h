#include "dtp.h"
void uart_init ( void );
int uart_probe(struct dtn *node);
char uart_recv ( void );
void uart_send ( char c );
void uart_send_string(char* str);
int read_line(char buf[], int buf_size);
int uart_read_int(void);
void reverse(char *str,int index);
void uart_send_int(int number);
void uart_send_uint(unsigned int number);
void uart_send_long(long number);
void uart_send_ulong(unsigned long number);

