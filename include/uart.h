#ifndef __UART_H__
#define __UART_H__

#include "gpio.h"
#include "irq.h"

#define UART_BUFFER_SIZE 2048

void uart_init();
void uart_send(unsigned int c);
char uart_get();
void uart_puts(char *s);
void uart_puts_bySize(char *s, int size);
int uart_get_int();
void uart_printint(unsigned long long int num);
void uart_printhex(unsigned long long int num);
void uart_irq();
void uart_send_string(char *);
void enable_uart_interrupt();

#endif