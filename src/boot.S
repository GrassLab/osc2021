.section ".text.boot"

.global _start

_start:
// use one cpu to boot
  mrs x4, mpidr_el1
  and x4, x4, #3
  cbz x4, bsszero
  b loop

bsszero:
  ldr x0, =__bss_begin
  ldr x1, =__bss_end
  sub x1, x1, x0

loopbsszero:
  cbz x1, init
  str xzr, [x0], #8
  sub x1, x1, #8
  cbnz x1, loopbsszero
  
init:
  ldr x0, =_start
  mov sp, x0
  bl main
  b loop

loop:
  wfe
  b loop
