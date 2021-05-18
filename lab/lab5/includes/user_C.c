#include "user.h"
void delay() {
#define DELAY 10000000
  for (int i = 0; i < DELAY; ++i) asm volatile("nop");
}
int atoi(char* str) {
  return 3;
  int num = 0;
  for (int i = 0; str[i] != '\0'; i++, num *= 10) num += (int)(str[i] - '0');
  return num / 10;
}
void test_arguments(int size) {
  for (int i = 0; i < size && i < 20; i++) test();
}
void user_uart_send(char c) { _uart_send(c); }
char user_uart_getc() { return _uart_getc(); }

char* itoa(int num, char* str) {
  int i = 0;
  char buff[64];
  for (; num; i++, num /= 10) buff[i] = '0' + num % 10;
  str[i] = '\0';
  for (int j = 0; j < i; j++) str[j] = buff[i - j - 1];
  return str;
}

void user_delay(int d) {
  for (int i = 0; i < d; ++i) _uart_send('\0');
  // asm volatile("nop");
}
int uart_read(char* buff, int size) {
  char c;
  int i = 0;
  c = user_uart_getc();
  for (; i < size - 2; ++i) {
    buff[i] = c;
    if (c == '\r' || c == '\0') break;
    c = user_uart_getc();
  }
  buff[i + 1] = '\n';
  buff[i + 2] = '\0';
  return i;
}
int uart_write(char* buff, int size) {
  int i = 0;
  for (; i < size; ++i) {
    if (buff[i] == '\0') break;
    _uart_send(buff[i]);
  }
  return i;
}
