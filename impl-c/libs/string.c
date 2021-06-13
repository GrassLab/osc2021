#include "string.h"

#include "config.h"
#include "test.h"

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
    *(out++) = *(src++);
    if (!*src) {
      *out = '\0';
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

const char *strchr(const char *s, const char c) {
  const char *p;
  if (s == NULL) {
    return NULL;
  }
  for (p = s; *p != '\0'; p++) {
    if (*p == c) {
      return p;
    }
  }
  return NULL;
}

const char *ignore_leading(const char *s, const char c) {
  const char *p;
  for (p = s; *p != '\0'; p++) {
    if (*p != c) {
      return p;
    }
  }
  return NULL;
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

#ifdef CFG_RUN_LIB_STRING_TEST
bool test_strchr() {
  {
    char *str = "abcc";
    const char *next = strchr(str, 'c');
    assert((*next == str[2]));
  }
  {
    char *str = "abcc";
    const char *next = strchr(str, 'd');
    assert(next == NULL);
  }
  {
    char *str = NULL;
    const char *next = strchr(str, 'd');
    assert(next == NULL);
  }
  {
    char *str = "";
    const char *next = strchr(str, 'd');
    assert(next == NULL);
  }
  return true;
}

bool test_ignore_leading() {
  {
    char *str = "aad";
    const char *next = ignore_leading(str, 'a');
    assert(next == &str[2]);
  }
  {
    char *str = "aaa";
    const char *next = ignore_leading(str, 'a');
    assert(next == NULL);
  }
  {
    char *str = "";
    const char *next = ignore_leading(str, 'a');
    assert(next == NULL);
  }
  return true;
}
#endif

void test_string() {
#ifdef CFG_RUN_LIB_STRING_TEST
  unittest(test_strchr, "LIB", "String - strchr");
  unittest(test_ignore_leading, "LIB", "String - ignore_leading");

#endif
}
