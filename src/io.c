#include "io.h"

#include "uart.h"

#define LOGGING

// modified for pwntools serial

void putc(char c) { uart_put_raw((unsigned int)(c)); }

void puts(const char *s) {
  print(s);
  putc('\n');
}

void puts_n(const char *s, unsigned long len) {
  print_n(s, len);
  putc('\n');
}

char getc() { return (char)uart_get_raw(); }

void gets(char *buffer) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n') {
    buffer[cursor] = c;
    cursor++;
  }
  buffer[cursor] = 0;
}

void gets_n(char *buffer, unsigned long len) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n' && cursor < len) {
    buffer[cursor] = c;
    cursor++;
  }
  buffer[cursor] = 0;
}

unsigned long long recv_ll() {
  unsigned long long data;
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 8; i++) {
    *itr++ = (char)uart_get_raw();
  }
  return data;
}

unsigned int recv_l() {
  unsigned int data;
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 4; i++) {
    *itr++ = (char)uart_get_raw();
  }
  return data;
}

void send_l(unsigned int data) {
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 4; i++) {
    uart_put_raw(*itr++);
  }
}

void send_ll(unsigned long long data) {
  unsigned char *itr = (unsigned char *)&data;
  for (int i = 0; i < 8; i++) {
    uart_put_raw(*itr++);
  }
}

void log(const char *msg) {
#ifdef LOGGING
  print(msg);
#endif
}

void log_hex(unsigned long long num) {
#ifdef LOGGING
  print_hex_ll(num);
#endif
}

void print(const char *s) {
  while (*s) {
    putc(*s++);
  }
}

void print_n(const char *s, unsigned long len) {
  unsigned long i = 0;
  while (*s && i < len) {
    putc(*s++);
    i++;
  }
}


void print_hex_ll(unsigned long long num) {
  print("0x");
  unsigned char *n = (unsigned char *)&num;
  for(int i = 7; i >= 0; i--) {
    print_hex_c(n[i] / 16);
    print_hex_c(n[i] % 16);
  }
}

void print_hex_l(unsigned int num) {
  print("0x");
  unsigned char *n = (unsigned char *)&num;
  for(int i = 3; i >= 0; i--) {
    print_hex_c(n[i] / 16);
    print_hex_c(n[i] % 16);
  }
}

void print_hex_c(unsigned char c) {
  if(c > 9) {
    putc('a' + c - 10);
  } else {
    putc('0' + c);
  }
}