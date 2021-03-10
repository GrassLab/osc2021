.global _start
.section .bl_relo

_start:
  adr x20, .
  ldr x21, =_blsize
  add x21, x21, x20
  ldr x22, =_stext

# relocate bootloader
loop0:
  cmp x20, x21
  b.eq loop0_end
  ldr x23, [x20]
  str x23, [x22]
  add x22, x22, #8
  add x20, x20, #8
  b loop0
loop0_end:
  ldr x0, =_bl_entry
  br x0


.section .text
_start_bl:
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

