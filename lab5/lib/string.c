#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>
#include <mm.h>

int strncmp(const char *a, const char *b, size_t n) {
  size_t i = 0;
  while (i < n-1 && a[i] == b[i] && a[i] != '\0' && b[i] != '\0') i++;
  return a[i] - b[i];
}

int strcmp(const char *a, const char *b) {
  int i = 0;
  while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i]) i++;
  return a[i] - b[i];
}

size_t strlen(const char *s) {
  size_t i = 0;
  while (s[i]) i++;
  return i;
}

void memcpy(void *dst, const void *src, size_t n) {
  char *_dst = dst;
  const char *_src = src;

  while(n--) {
    *_dst++ = *_src++;
  }
}

char *strdup(const char *s) {
  int len = strlen(s) + 1;
  char *p = kmalloc(len);
  memcpy(p, s, len);

  return p;
}

void *memset(void *s, int c, size_t n) {
  char *p = s;
  for (size_t i = 0; i < n; i++) {
    p[i] = c;
  }
  return s;
}

#endif