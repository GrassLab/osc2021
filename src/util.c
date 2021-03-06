#include "util.h"
#include "io.h"

extern void _halt();

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

void wait_clock(unsigned int t) {
  t >>= 2;
  while (t--)
    ;
}

void system_error(const char * msg) {
  puts(msg);
  _halt();
}