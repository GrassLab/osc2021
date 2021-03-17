#include "util.h"
#include "io.h"

int strcmp(const char *p1, const char *p2) {
  const unsigned char *s1 = (const unsigned char *)p1;
  const unsigned char *s2 = (const unsigned char *)p2;
  unsigned char c1, c2;
  do {
    c1 = (unsigned char)*s1++;
    c2 = (unsigned char)*s2++;
    if (c1 == '\0') return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

void *memcpy(void *dest, const void *src, size_t len) {
  char *d = dest;
  const char *s = src;
  while (len--) *d++ = *s++;
  return dest;
}

int strcmp_n(const char *s1, const char *s2, size_t n) {
  while (n && *s1 && (*s1 == *s2)) {
    ++s1;
    ++s2;
    --n;
  }
  if (n == 0) {
    return 0;
  } else {
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
  }
}

char *strcpy(char *destination, const char *source) {
  if (destination == NULL) return NULL;

  char *ptr = destination;
  while (*source != '\0') {
    *destination = *source;
    destination++;
    source++;
  }
  *destination = '\0';
  return ptr;
}

char *strcpy_n(char *destination, const char *source, size_t len) {
  if (destination == NULL) return NULL;
  char *ptr = destination;
  while (*source != ' ') {
    *destination = *source;
    destination++;
    source++;
    len--;
    if (!len) {
      break;
    }
  }
  *destination = '\0';
  return ptr;
}

size_t strlen(const char *str) {
  const char *s;
  for (s = str; *s; ++s)
    ;
  return (s - str);
}

long long atoi(char *s) {
  if (*s == '-') {
    return -1 * (long long)atol(s + 1);
  }
  return (long long)atol(s);
}

long long atoi_n(char *s, size_t max_len, int base) {
  if (max_len == 0) return 0;
  if (*s == '-') {
    return -1 * (long long)atol_n(s + 1, max_len - 1, base);
  }
  return (long long)atol_n(s, max_len, base);
}

unsigned long long atol(char *s) { return atol_n(s, 21, 10); }

unsigned long long atol_n(char *s, size_t max_len, int base) {
  unsigned long long num = 0;
  while (*s && max_len) {
    unsigned long long n = 0;
    // putc(*s);
    if (*s >= '0' && *s <= '9') {
      n = *s - '0';
    } else if (*s >= 'a' && *s <= 'f') {
      n = *s - 'a' + 10;
    } else if (*s >= 'A' && *s <= 'F') {
      n = *s - 'A' + 10;
    }
    num = num * (unsigned long long)base + n;
    max_len--;
    s++;
  }
  // putc('\n');
  return num;
}