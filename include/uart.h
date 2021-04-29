#ifndef _UART_H_
#define  _UART_H_

#include "queue.h"
#include "type.h"

typedef queue_t io_t;

extern io_t *uart_in;
extern io_t *uart_out;

extern bool_t end_of_input;

void uart_init();
char uart_getc();
void uart_putc(char);
void async_write(const char*);
void async_read(char*);
int buffer_push(char, io_t*);
char buffer_pop(io_t*);
void buffer_flush(io_t*);
bool_t buffer_empty(io_t*);
bool_t buffer_full(io_t*);
//======================
void print(const char*);
void print_int(unsigned long long);
void print_sysreg(unsigned long long,
                  unsigned long long,
                  unsigned long long);
void print_timer(unsigned long long,
                 unsigned long long);

#endif
