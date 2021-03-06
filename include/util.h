#ifndef UTIL_H
#define UTIL_H

int strcmp(const char *p1, const char *p2);
void wait_clock(unsigned int t);
void system_error(const char * msg);
void uart_flush_stdin();

#endif