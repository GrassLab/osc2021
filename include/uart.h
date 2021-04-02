#ifndef UART_H
#define UART_H

void uart_init();
// blocking uart recv unsigned char
unsigned char uart_recv_c();
// blocking uart send unsigned char
void uart_send_c(unsigned char data);
// wait for uart FIFO buffer clear
void flush();

// blocking uart recv unsigned int
unsigned int uart_recv_i();
// blocking uart recv unsigned long
unsigned long uart_recv_l();

// blocking uart send unsigned int
void uart_send_i(unsigned int data);
// blocking uart send unsigned long
void uart_send_l(unsigned long data);

#endif