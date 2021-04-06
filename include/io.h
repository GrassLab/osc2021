#ifndef IO_H
#define IO_H

// all function has been adapted to use uart

void putc(char c);
void puts(const char *s);
void puts_n(const char *s, unsigned long len);

char getc();
void gets(char *buffer);
void gets_n(char *buffer, unsigned long len);

unsigned long long recv_ll();
unsigned int recv_l();

void send_l(unsigned int data);
void send_ll(unsigned long long data);

void log(const char *msg);
void log_hex(unsigned long long num);

void print(const char *s);
void print_n(const char *s, unsigned long len);

void print_hex_ll(unsigned long long num);
void print_hex_l(unsigned int num);
void print_hex_c(unsigned char c);


#endif