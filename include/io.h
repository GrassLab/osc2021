#ifndef IO_H
#define IO_H

// all function has been adapted to use uart
void putc(char c);
void puts(const char *s);
void puts_n(const char *s, unsigned int max_length);
char getc();
void gets(char *buffer);
void gets_n(char *buffer, unsigned int max_length);
unsigned long long recv_ll();
unsigned long recv_l();
void send_l(unsigned long data);
void send_ll(unsigned long long data);

#endif