.global _start
.section ".text"

_start:
  ldr x20, =_sbss
  ldr x21, =_ebss

# clear bss
loop1:
  cmp x20, x21
  b.eq loop1_end
  str xzr, [x20]
  add x20, x20, #8
  b loop1
loop1_end:

  ldr x20, =_estack
  mov sp, x20

  bl main

loop:
  wfe
  b loop
