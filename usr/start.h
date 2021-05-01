#ifndef START_H
#define START_H

#include <stddef.h>

unsigned long read(unsigned long fd, char *buf, unsigned long size);
unsigned long write(unsigned long fd, const char *buf, unsigned long size);
unsigned long get_pid();
unsigned long fork();
unsigned long execve(const char *name, char *const argv[]);
void exit();

size_t strlen(const char *str) {
  const char *s;
  for (s = str; *s; ++s)
    ;
  return (s - str);
}

void print(const char *s) { write(1, s, strlen(s)); }

void puts(const char *s) {
  print(s);
  print("\n");
}

void print_hex(unsigned long num) {
  print("0x");

  if (num == 0) {
    print("0");
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

void delay(unsigned long t) {
  t >>= 2;
  while (t--) {
    asm volatile("");
  }
}

#endif