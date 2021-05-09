#include "string.h"

int strcmp(const char *x, const char *y) {
  while (*x) {
    if (*x != *y)
      break;
    x++, y++;
  }
  return *(const unsigned char *)x - *(const unsigned char *)y;
}

int strncmp(const char *x, const char *y, size_t n) {
  while (n--) {
    if (*x != *y)
      return 1;
  }
  return 0;
}

void memcpy(char *dst, const char *src, size_t n) {
  for (; n > 0; n--) {
    *(dst++) = *(src++);
  }
}

char *strcpy(char *dst, const char *src) {
  char *out = dst;
  while (1) {
    *(dst++) = *(src++);
    if (!*src) {
      *dst = '\0';
      break;
    }
  }
  return out;
}

int strlen(const char *str) {
  const char *s;
  for (s = str; *s; ++s) {
  }
  return (s - str);
}

// Credits to
// https://stackoverflow.com/questions/3982320/convert-integer-to-string-without-access-to-libraries
// Caution:
//  1. Copy the string returned if you need to store result
//  2. Only pass in positive values
char *itoa(int64_t val, int base) {
  static char buf[64] = {0};
  int i = 62;
  if (val == 0) {
    buf[i] = '0';
    return &buf[i];
  }

  for (; val && i; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
  return &buf[i + 1];
}