#include "mem.h"

.section ".boot"

.globl _start
_start:
  mrs x0, mpidr_el1
  and x0, x0, #0xFF
  cbz x0, mcore
  b loop

loop:
  b loop

mcore:
  adr x0, bss_begin
  adr x1, bss_end
  sub x1, x1, x0
  bl clearmem
  ldr x0, = _stack_top
  mov sp, x0
  bl kernel



  