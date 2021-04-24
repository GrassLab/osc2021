#ifndef _PRINTF_H_
#define _PRINTF_H_

#define STRBUF_SIZE 0x30

int printf(const char *format, ...);
void ltoa(unsigned long val, char *buf);
void ftoa(double val, char *buf);
void fdivtoa(unsigned long dividend, unsigned long divisor, char *buf);
void putstr(char *s);
void puthex(unsigned int d);
#endif