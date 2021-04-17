#include "gpio.h"

void wait_clock(unsigned long t) {
  t >>= 2;
  while (t--){
    asm volatile("");
  }
}