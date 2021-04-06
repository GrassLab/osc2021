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

void *memcpy(void *dst, const void *src, size_t len) {
  char *d = dst;
  const char *s = src;
  while (len--) *d++ = *s++;
  return dst;
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

long long atoi(const char *s) {
  if (*s == '-') {
    return -1 * (long long)atol(s + 1);
  }
  return (long long)atol(s);
}

long long atoi_n(const char *s, size_t len, int base) {
  if (len == 0) return 0;
  if (*s == '-') {
    return -1 * (long long)atol_n(s + 1, len - 1, base);
  }
  return (long long)atol_n(s, len, base);
}

unsigned long long atol(const char *s) { return atol_n(s, 21, 10); }

unsigned long long atol_n(const char *s, size_t len, int base) {
  unsigned long long num = 0;
  while (*s && len) {
    unsigned long long n = 0;
    if (*s >= '0' && *s <= '9') {
      n = *s - '0';
    } else if (*s >= 'a' && *s <= 'f') {
      n = *s - 'a' + 10;
    } else if (*s >= 'A' && *s <= 'F') {
      n = *s - 'A' + 10;
    }
    num = num * (unsigned long long)base + n;
    len--;
    s++;
  }
  return num;
}

char *new_str(char *src) {
  size_t len = strlen(src);
  char *str = (char *)kmalloc(len + 1);
  strcpy(str, src);
  return str;
}

void init_list(list_head *l) {
  l->bk = l;
  l->fd = l;
}

void push_list(list_head *l, list_head *chunk) {
  chunk->bk = l;
  chunk->fd = l->fd;
  l->fd->bk = chunk;
  l->fd = chunk;
}

void pop_list(list_head *chunk) {
  chunk->fd->bk = chunk->bk;
  chunk->bk->fd = chunk->fd;
}

int list_empty(list_head *l) { return l->fd == l; }