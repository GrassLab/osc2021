#include "timer.h"
#include <types.h>
#include <uart.h>
#include <printf.h>
void get_core_time() {
  size_t time;
  asm volatile("mrs  %[result], cntpct_el0": [result]"=r"(time));
  printf("core timer interrupt: %x\n", time);
  //asm volatile("mrs  %[result], cntfrq_el0": [result]"=r"(time));
}