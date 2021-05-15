#ifndef START_H
#define START_H

#include <stdarg.h>
#include <stddef.h>

unsigned long uart_read(char buf[], size_t size);
unsigned long uart_write(const char buf[], size_t size);
int getpid();
unsigned long fork();
int exec(const char *name, char *const argv[]);
void exit();

void delay(int n) {
  while (n--)
    ;
}

int strlen(const char *s) {
  int len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

void print_c(char c) {
  char s[2];
  s[0] = c;
  s[1] = '\0';
  uart_write(s, strlen(s));
}

void print_s(char *s) { uart_write(s, strlen(s)); }

void print_i(int x) {
  if (x < 0) {
    print_c('-');
    x = -x;
  }
  if (x >= 10) print_i(x / 10);
  print_c(x % 10 + '0');
}

void print_h(unsigned long x) {
  print_s("0x");
  for (int c = 28; c >= 0; c -= 4) {
    int n = (x >> c) & 0xF;
    n += n > 9 ? 'A' - 10 : '0';
    print_c(n);
  }
}

#endif