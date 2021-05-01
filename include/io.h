#ifndef IO_H
#define IO_H

#include "uart.h"

#define LOG_DEBUG 0
#define LOG_ERROR 1
#define LOG_PRINT 2

extern unsigned long nonblock;

void putc(char c);
void puts(const char *s);
void puts_n(const char *s, unsigned long len);

char getc();
void gets(char *buffer);
void gets_n(char *buffer, unsigned long len);

void log(const char *msg, int flag);
void log_hex(const char *msg, unsigned long num, int flag);

void print(const char *s);
void print_n(const char *s, unsigned long len);
void print_hex(unsigned long num);

void init_nonblock_io();

void uart_handler();

unsigned long recv_buf_empty();

#endif