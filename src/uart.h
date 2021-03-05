#ifndef UART_H
#define UART_H

void uart_init ();
void uart_sendc (char c);
void uart_sendi (long num);
void uart_sendf (float num);
void uart_sendh (unsigned long num);
void uart_send (char *str);
char uart_getc ();
void uart_getline (char *buffer, unsigned int size);

#endif
