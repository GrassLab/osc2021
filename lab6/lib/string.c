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

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  for ( ; i < n; i++)
    dst[i] = '\0';

  return dst;
}

char *strcpy(void *dst, const void *src) {
  char *_dst = dst;
  const char *_src = src;

  while(*_src) {
    *_dst++ = *_src++;
  }

  *_dst = '\0';

  return dst;
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

char *strchr(const char *s, int c) {
  int i = 0;
  if (c == '\0') {
    return (char *)s + strlen(s);
  }

  while (s[i]) {
    if (s[i] == c) {
      return (char *)&s[i];
    }
    i++;
  }

  return NULL;
}

char *strsep(char **stringp, const char *delim) {
  char *p = *stringp;
  if (p == NULL) {
    return NULL;
  }

  *stringp = NULL;

  char *p_save = p;
  char *end = p + strlen(p);
  size_t delim_len = strlen(delim);
  int check;

  while (p < end) {
    check = 0;

    for (size_t i = 0; i < delim_len; i++) {
      if (*p == delim[i]) {
        *p = '\0';
        *stringp = p + 1;
        check = 1;
        break;
      }
    }

    if (check) break;
    p++;
  }

  return p_save;
}

#endif