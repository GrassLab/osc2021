#ifndef START_H
#define START_H

#include <stdarg.h>
#include <stddef.h>

#define O_CREAT 1

unsigned long uart_read(char buf[], size_t size);
unsigned long uart_write(const char buf[], size_t size);
int getpid();
unsigned long fork();
int exec(const char *name, char *const argv[]);
void exit();
int open(const char *pathname, int flags);
int close(int fd);
int write(int fd, const void *buf, int count);
int read(int fd, void *buf, int count);
int list(int fd, void *buf, int index);
int mkdir(const char *pathname);
int chdir(const char *pathname);
int mount(const char* device, const char* mountpoint, const char* filesystem);
int umount(const char* mountpoint);

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

int strncmp(const char *s1, const char *s2, unsigned n) {
  unsigned char c1 = '\0';
  unsigned char c2 = '\0';
  if (n >= 4) {
    unsigned n4 = n >> 2;
    do {
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
    } while (--n4 > 0);
    n &= 3;
  }
  while (n > 0) {
    c1 = (unsigned char)*s1++;
    c2 = (unsigned char)*s2++;
    if (c1 == '\0' || c1 != c2) return c1 - c2;
    n--;
  }
  return c1 - c2;
}

#endif