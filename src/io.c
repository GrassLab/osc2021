#include "io.h"

#include "uart.h"

#define LOGGING_DEBUG
#define LOGGING_ERROR
#define LOGGING_PRINT

void putc(char c) { uart_send_c((unsigned char)(c)); }

void puts(const char *s) {
  print(s);
  putc('\n');
}

void puts_n(const char *s, unsigned long len) {
  print_n(s, len);
  putc('\n');
}

char getc() { return (char)uart_recv_c(); }

void gets(char *buffer) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n') {
    buffer[cursor++] = c;
  }
  buffer[cursor] = 0;
}

void gets_n(char *buffer, unsigned long len) {
  unsigned long cursor = 0;
  char c;
  while ((c = getc()) != '\n' && cursor < len) {
    buffer[cursor++] = c;
  }
  buffer[cursor] = 0;
}

void log(const char *msg, int flag) {
  if (flag == LOG_DEBUG) {
#ifdef LOGGING_DEBUG
    print(msg);
#endif
  } else if (flag == LOG_ERROR) {
#ifdef LOGGING_ERROR
    print(msg);
#endif
  } else if (flag == LOG_PRINT) {
#ifdef LOGGING_PRINT
    print(msg);
#endif
  }
}

void log_hex(const char *msg, unsigned long num, int flag) {
  if (flag == LOG_DEBUG) {
#ifdef LOGGING_DEBUG
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  } else if (flag == LOG_ERROR) {
#ifdef LOGGING_ERROR
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  } else if (flag == LOG_PRINT) {
#ifdef LOGGING_PRINT
    print(msg);
    putc(' ');
    print_hex(num);
    putc('\n');
#endif
  }
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

void print_hex(unsigned long num) {
  print("0x");

  if (num == 0) {
    putc('0');
    return;
  }

  char buf[17];
  for (int i = 15; i >= 0; i--) {
    unsigned char c = (num & 15);
    if (c > 9) {
      buf[i] = 'a' + (c - 10);
    } else {
      buf[i] = '0' + c;
    }
    num = num >> 4;
  }
  buf[16] = 0;

  for (int i = 0; i < 16; i++) {
    if (buf[i] != '0') {
      print(&(buf[i]));
      break;
    }
  }
}
