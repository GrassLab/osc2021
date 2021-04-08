#include "timer.h"
#include <types.h>
#include <uart.h>
#include <printf.h>
void get_core_time() {
  size_t time, count, freq;
  asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(count));
  asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(freq));
  time = count / freq;
  printf("core timer interrupt: %x\n", time);
}
