#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>

int strncmp(const char *a, const char *b, size_t n) {
  int i = 0;
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

#endif