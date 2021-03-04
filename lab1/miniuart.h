#ifndef __MINIUART_H
#define __MINIUART_H

void miniuart_init();
char miniuart_recv();
void miniuart_send_C(char c);
void miniuart_send_S(const char *str);

#endif
