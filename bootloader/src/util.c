#include "util.h"
#include "mmio.h"
#include "shell.h"

char *gets(char *str) {
  char c = '\0';
  // char buff[GETS_BUFF_LEN];
  int buff_end = 0;
  do {
    c = uart_getc();
    /*
    int test = (int)c;
    while(test) {
      uart_setc((test&1)+'0');
      test = test >> 1;
    }
    uart_setc('\n');
    */
    if (c == 0x08 || c == 0x7f) { // backspace, ^H works on screen, BS will send DEL signal
      if (buff_end == 0) continue; // boundary check
      puts("\b \b");
      --buff_end;
      //buff_end = (--buff_end & (1<<31)) ? 0 : buff_end;
      continue;
    }
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
