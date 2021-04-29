#include "util.h"

#include "io.h"
#include "mem.h"

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
  }
  return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

int strcmp_n(const char *s1, const char *s2, size_t n) {
  while (--n && *s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
  }
  return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void *memcpy(void *dst, const void *src, size_t len) {
  char *d = dst;
  const char *s = src;
  while (len--) *d++ = *s++;
  return dst;
}

void *memcpy_ul(void *dst, const void *src, size_t len) {
  len /= sizeof(unsigned long);
  unsigned long *d = dst;
  const unsigned long *s = src;
  while (len--) *d++ = *s++;
  return dst;
}

void *memset_ul(void *dst, unsigned long data, size_t len) {
  len /= sizeof(unsigned long);
  unsigned long *d = dst;
  while (len--) *d++ = data;
  return dst;
}

char *strcpy(char *dst, const char *src) {
  if (dst == NULL) return NULL;
  char *ptr = dst;
  while (*src != 0) {
    *dst++ = *src++;
  }
  *dst = 0;
  return ptr;
}

char *strcpy_n(char *dst, const char *src, size_t len) {
  if (dst == NULL) return NULL;
  char *ptr = dst;
  while (len != 0 && *src != 0) {
    --len;
    *dst++ = *src++;
  }
  *dst = 0;
  return ptr;
}

size_t strlen(const char *str) {
  const char *s;
  for (s = str; *s; ++s)
    ;
  return (s - str);
}

long atoi(const char *s) {
  if (*s == '-') {
    return -1 * (long)atol(s + 1);
  }
  return (long)atol(s);
}

long atoi_n(const char *s, size_t len, size_t base) {
  if (len == 0) return 0;
  if (*s == '-') {
    return -1 * (long)atol_n(s + 1, len - 1, base);
  }
  return (long)atol_n(s, len, base);
}

size_t atol(const char *s) { return atol_n(s, 23, 10); }

size_t atol_n(const char *s, size_t len, size_t base) {
  if (base == 16) {
    if (strcmp_n(s, "0x", 2) == 0) {
      s += 2;
    }
  }

  size_t num = 0;
  while (len && *s) {
    size_t n = 0;
    char c = *s;
    if (c >= '0' && c <= '9') {
      n = c - '0';
    } else if (c >= 'a' && c <= 'f') {
      n = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
      n = c - 'A' + 10;
    }
    num = num * base + n;
    len--;
    s++;
  }
  return num;
}

char *new_str(const char *src) {
  size_t len = strlen(src);
  char *str = (char *)kmalloc(len + 1);
  strcpy(str, src);
  return str;
}

char *split_str(char *s) {
  char *right = NULL;
  while (*s) {
    if (*s == ' ') {
      right = s + 1;
      *s = 0;
      break;
    }
    s++;
  }
  return right;
}

size_t cnt_white(const char *s) {
  unsigned long cnt = 0;
  while (*s) {
    if (*s == ' ') {
      cnt++;
    }
    s++;
  }
  return cnt;
}