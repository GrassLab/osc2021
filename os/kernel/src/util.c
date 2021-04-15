#include "util.h"
#include "mmio.h"
#include "shell.h"

char *gets(char *str) {
  char c = '\0';
  // char buff[GETS_BUFF_LEN];
  int buff_end = 0;
  do {
    c = uart_getc();
    if (c == 0x08 ||
        c == 0x7f) { // backspace, ^H works on screen, BS will send DEL signal
      if (buff_end == 0)
        continue; // boundary check
      puts("\b \b");
      buff_end--;
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

int putln(const char *str) {
  if (!puts(str)) {
    return 1;
  }
  if (!puts("\n\r")) {
    return 1;
  }
  return 0;
}

// 0 as same, 1 as different, -1 as error
int strcmp(const char *str1, const char *str2) {
  long cnt = 0;
  while (str1[cnt] != '\0' && str2[cnt] != '\0') {
    if (str1[cnt] != str2[cnt]) {
      return 1;
    }
    if (cnt >= 1000) {
      return -1;
    }
    cnt++;
  }
  if (str1[cnt] == '\0' && str2[cnt] == '\0') {
    return 0;
  }
  return 1;
}

// 0 as same, 1 as different, -1 as error
int strncmp(const char *str1, const char *str2, unsigned long num) {
  long cnt = 0;
  while (str1[cnt] != '\0' && str2[cnt] != '\0' && cnt < num) {
    if (str1[cnt] != str2[cnt]) {
      return 1;
    }
    if (cnt >= 1000) {
      return -1;
    }
    cnt++;
  }
  if (cnt >= num) {
    return 0;
  }
  // if cnt >= num
  if (str1[cnt] == '\0' && str2[cnt] == '\0') {
    return 0;
  }
  return 1;
}
