#ifndef __UART_H_
#define __UART_H_

void mini_uart_init();
void read_uart(char *buffer, int count);
int readline_uart(char *buffer);
void write_uart(const char *buffer, int count);
void writeline_uart(const char *buffer, int count);
int interact_readline_uart(char *buffer);
void puts_uart(const char *buffer);
void write_num_uart(unsigned num);

#endif
