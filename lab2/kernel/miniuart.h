#ifndef __MINIUART_H
#define __MINIUART_H

#include <stddef.h>
#include "bcm2837.h"

void miniuart_init();
char miniuart_recv();
void miniuart_send_C(char c);
void miniuart_send_nC(char c, size_t n);
void miniuart_send_S(const char *str);
void miniuart_send_nS(const char *str, size_t n);

#endif
