#include "io.h"

#include "uart.h"

void putc(char c) {
  if (c == '\n') {
    uart_put_raw((unsigned int)('\r'));
  }
  uart_put_raw((unsigned int)(c));
}

void puts(const char *s) {
  while (*s) {
    putc(*s++);
  }
}

void puts_n(const char *s, unsigned int max_length) {
  unsigned int i = 0;
  while (*s && i < max_length) {
    putc(*s++);
    i++;
  }
}

char getc() {
  char c = (char)uart_get_raw();
  if (c == 127) {
    return '\b';
  } else if (c == '\r') {
    c = '\n';
  }
  putc(c);
  return c;
}

void gets(char *buffer) {
  unsigned int cursor = 0;
  char c;
  while ((c = getc()) != '\n') {
    if (c == 248) {
      continue;
    } else if (c == '\b') {
      if (cursor > 0) {
        puts("\b \b");
        cursor--;
      }
    } else {
      buffer[cursor] = c;
      cursor++;
    }
  }
  buffer[cursor] = 0;
}

void gets_n(char *buffer, unsigned int max_length) {
  unsigned int cursor = 0;
  char c;
  while ((c = getc()) != '\n' && cursor < max_length) {
    if (c == 248) {
      continue;
    } else if (c == '\b') {
      if (cursor > 0) {
        puts("\b \b");
        cursor--;
      }
    } else {
      buffer[cursor] = c;
      cursor++;
    }
  }
  buffer[cursor] = 0;
}

unsigned long long recv_ll() {
  unsigned long long data;
  unsigned char *itr = (unsigned char *)&data;
  for(int i = 0; i < 8; i++) {
    *itr++ = (char)uart_get_raw(); 
  }
  return data;
}

unsigned long recv_l() {
  unsigned long data;
  unsigned char *itr = (unsigned char *)&data;
  for(int i = 0; i < 4; i++) {
    *itr++ = (char)uart_get_raw(); 
  }
  return data;
}

void send_l(unsigned long data) {
  unsigned char *itr = (unsigned char *)&data;
  for(int i = 0; i < 4; i++) {
    uart_put_raw(*itr++);
  }
}

void send_ll(unsigned long long data) {
  unsigned char *itr = (unsigned char *)&data;
  for(int i = 0; i < 8; i++) {
    uart_put_raw(*itr++);
  }
}