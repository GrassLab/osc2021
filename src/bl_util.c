#include "bl_util.h"

#include "io.h"

extern void _halt();

void wait_clock(unsigned long t) {
  t >>= 2;
  while (t--)
    ;
}

void sys_error(const char *msg) {
  log("sys error: ");
  log(msg);
  log("\n");
  _halt();
}