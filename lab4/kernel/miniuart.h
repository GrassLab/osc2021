#ifndef __MINIUART_H
#define __MINIUART_H

#include <stdint.h>
#include <stddef.h>
#include "bcm2837.h"
#include "dtb.h"

void miniuart_init();
char miniuart_recv();
uint32_t miniuart_send_C(char c);
uint32_t miniuart_send_nC(char c, size_t n);
uint32_t miniuart_send_S(const char *str);
uint32_t miniuart_send_nS(const char *str, size_t n);

#endif
