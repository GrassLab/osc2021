#include "bl_util.h"

#include "io.h"

extern void _halt();

void wait_clock(unsigned int t) {
  t >>= 2;
  while (t--)
    ;
}

void system_error(const char *msg) {
  puts("Error :");
  puts(msg);
  puts("\n");
  _halt();
}