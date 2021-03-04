#ifndef _MINI_UART_H
#define _MINI_UART_H

void init_uart();
void send_char(char c);
void send_string(char *);
char receive_char();

#endif