#include "util.h"
#include "mmio.h"

char *gets(char *str) {
  char c = '\0';
  // char buff[GETS_BUFF_LEN];
  int buff_end = 0;
  do {
    c = uart_getc();
    str[buff_end++] = c;
    uart_setc(c);
  } while (c != '\n');
  uart_setc('\r');
  str[buff_end - 1] = '\0';
  return str;
}

int puts(const char *str) {
  int str_end = 0;
  while (str[str_end] != '\0') {
    uart_setc(str[str_end++]);
  }
  return str_end;
}

// 0 as same, 1 as different, -1 as error
int strcmp(const char *a, const char *b) {
  int cnt = 0;
  while (a[cnt] != '\0' && b[cnt] != '\0') {
    if (a[cnt] != b[cnt]) {
      return 1;
    }
    if (cnt >= 1000) {
      return -1;
    }
    cnt++;
  }
  if (a[cnt] == '\0' && b[cnt] == '\0') {
    return 0;
  }
  return 1;
}
