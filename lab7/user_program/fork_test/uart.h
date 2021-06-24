#ifndef UART_H
#define UART_H

#include "mmio.h"

// uart interrupt enable 
// enable uart interrupt controller’s IRQs1(0x3f00b210)’s bit29. 
#define ENABLE_IRQS_1		((volatile unsigned int*)(MMIO_BASE+0x0000b210))
// disable uart interrupt controller’s IRQs1(0x3f00b210)’s bit29. 
#define DISABLE_IRQS_1		((volatile unsigned int*)(MMIO_BASE+0x0000b21c))
// set bit29 for enable or disable or check uart interrupt
#define AUX_IRQ 			(1 << 29)

// check irq interrupt type
// IRQ_PENDING_1 : uart pending 1 register , use bit29 check if irq interrupt
#define IRQ_PENDING_1		((volatile unsigned int*)(MMIO_BASE+0x0000b204))

#define MAX_UART_BUFFER		1024

void uart_init(); // init
void uart_sendchar(unsigned int c); // send char
char uart_getchar(); // get char
void uart_putstr(char *str); // send string

void uart_interrupt_init();
void enable_uart_interrupt();
void disable_uart_interrupt();
void enable_write_interrupt();
void disable_write_interrupt();
void uart_interrupt_handler();
char uart_async_getchar();
void uart_async_putstr(char *str);

#endif