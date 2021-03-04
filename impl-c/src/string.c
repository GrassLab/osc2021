#include "string.h"

int strcmp(const char *x, const char *y) {
  while (*x) {
    if (*x != *y)
      break;
    x++, y++;
  }
  return *(const unsigned char *)x - *(const unsigned char *)y;
}

// Credits to
// https://stackoverflow.com/questions/3982320/convert-integer-to-string-without-access-to-libraries
// Caution:
//  1. Copy the string returned if you need to store result
//  2. Only pass in positive values
char *itoa(int val, int base) {
  static char buf[32] = {0};
  int i = 30;
  if (val == 0) {
    buf[i] = '0';
    return &buf[i];
  }

  for (; val && i; --i, val /= base)
    buf[i] = "0123456789abcdef"[val % base];
  return &buf[i + 1];
}