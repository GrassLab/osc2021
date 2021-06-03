#include "io.h"

#include "mini_uart.h"
#include "utils.h"

char read_b() { return uart_getb(); }

char read_c() {
  char c = uart_async_getc();
  print_c(c);
  if (c == '\n') print_c('\r');
  return c;
}

char *read_s(char *str, int max_size) {
  for (int i = 0; i < max_size; i++) {
    str[i] = read_c();
    if (str[i] == 127) {  // delete
      i--;
      if (i >= 0) {
        print_s("\b \b");
        i--;
      }
    }
    if (str[i] == '\n' || str[i] == '\r') {
      str[i] = 0;
      break;
    }
  }
  str[max_size - 1] = 0;
  return str;
}

int read_i() {
  int x = 0, f = 0;
  char ch = 0;
  while (!is_digit(ch)) {
    f |= (ch == '-');
    ch = read_c();
  }
  while (is_digit(ch)) {
    x = x * 10 + (ch - '0');
    ch = read_c();
  }
  return f ? -x : x;
}

unsigned long long int read_h() {
  char str[11];
  char *value = read_s(str, 11);
  return hex2int(value + 2, 8);  // first two characters are '0x'
}

void print_c(char c) { uart_send(c); }

void print_s(char *s) { uart_puts(s); }

void print_i(int x) {
  if (x < 0) {
    print_c('-');
    x = -x;
  }
  if (x >= 10) print_i(x / 10);
  print_c(x % 10 + '0');
}

void print_h(uint32_t x) {
  print_s("0x");
  for (int c = 28; c >= 0; c -= 4) {
    int n = (x >> c) & 0xF;
    n += n > 9 ? 'A' - 10 : '0';
    print_c(n);
  }
}
