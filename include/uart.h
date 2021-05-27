#ifndef _UART_H_
#define  _UART_H_

#include "queue.h"
#include "type.h"

typedef queue_t io_t;

extern io_t *uart_in;
extern io_t *uart_out;

void uart_init();
char uart_getc();
void uart_putc(char);
void async_uart_putc(char);
size_t async_write(const char*, size_t);
size_t async_read(char*, size_t);
int buffer_push(char, io_t*);
char buffer_pop(io_t*);
void buffer_flush(io_t*);
bool_t buffer_empty(io_t*);
bool_t buffer_full(io_t*);
void buffer_lock(io_t*);
void buffer_unlock(io_t*);
//======================
void print(const char*);
void print_int(unsigned long long);
void print_hex(unsigned long long);
void print_sysreg(unsigned long long,
                  unsigned long long,
                  unsigned long long);
void print_timer(unsigned long long,
                 unsigned long long);

#endif
