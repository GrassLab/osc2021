#include "string.h"

int strcmp(const char *x, const char *y) {
  while (*x) {
    if (*x != *y)
      break;
    x++, y++;
  }
  return *(const unsigned char *)x - *(const unsigned char *)y;
}