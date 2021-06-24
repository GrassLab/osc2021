#ifndef START_H
#define START_H

#include <stddef.h>

typedef struct dirent {
  char name[512];
  unsigned long size;
  unsigned long mode;
} dirent;

unsigned long read(unsigned long fd, char *buf, unsigned long size);
unsigned long write(unsigned long fd, const char *buf, unsigned long size);
unsigned long open(const char *path, unsigned long flag);
int close(unsigned long fd_num);
unsigned long get_pid();
unsigned long fork();
unsigned long execve(const char *name, char *const argv[]);
void exit();
int mkdir(const char *path);
int rmdir(const char *path);
int create(const char *path);
int remove(const char *path);
int chdir(const char *path);
int mount(const char *dev, const char *mp, const char *fs);
int umount(const char *path);
int getdent(unsigned long fd_num, unsigned long count, dirent *dent);

#define O_CREAT 1 << 0

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

#define assert(x)     \
  if (!(x)) {         \
    print("error\n"); \
  }

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
  }
  return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) {
    return 0;
  }
  while (--n && *s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
  }
  return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

#endif