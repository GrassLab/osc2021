#include "io.h"

char buf[256];
void kernel() {
  puts("hi\n");
  gets_n(buf, 255);
  puts(buf);
}