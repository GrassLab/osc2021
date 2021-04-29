#include <stdarg.h>
#include <stdint.h>

#include "lib.h"
#define MX_BUF_SIZE 100

static char *itoa(int64_t val, int base);
static size_t sputs(char *output, const char *input);

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

size_t sputs(char *output, const char *input) {
  size_t size = 0;
  while (*input) {
    *(output++) = *(input++);
    size++;
  }
  *output = '\0';
  return size;
}

// same as println, but without a newline at the end
void printf(char *fmt, ...) {
  int64_t i;
  char *s;
  size_t size = 0;
  char buf[MX_BUF_SIZE] = {};

  va_list arg;
  va_start(arg, fmt);
  for (; fmt; fmt++) {
    // Send leading chars
    while (*fmt != '%' && *fmt) {
      buf[size++] = *fmt++;
    }
    if (*fmt == 0)
      break;
    // Start parsing %..
    fmt++;
    switch (*fmt) {
    case 'd':
      i = va_arg(arg, int);
      if (i < 0) {
        i = -i;
        size += sputs(&buf[size], "-");
      }
      size += sputs(&buf[size], itoa(i, 10));
      break;
    case 'x':
      i = va_arg(arg, int);
      size += sputs(&buf[size], "0x");
      size += sputs(&buf[size], itoa((int64_t)i, 16));
      break;
    case 's':
      s = va_arg(arg, char *);
      size += sputs(&buf[size], s);
      break;
    case '%':
      size += sputs(&buf[size], "%");
      break;
    }
  }
  buf[size] = 0;
  va_end(arg);
  // syscall
  uart_write(buf, size);
}