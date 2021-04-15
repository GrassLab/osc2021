.section ".text.boot"

.globl _start
_start:
  // disable multi core
  mrs   x0, mpidr_el1
  and   x0, x0, #3
  cbnz  x0, _hang
  // Set stack pointer before code session
  ldr   x0, =_start
  mov   sp, x0
  // Set bss as zero
  ldr   x0, =__bss_begin
  ldr   x1, =__bss_end
  bl    _bss_clear
  // Jump to kernel
  bl    bootloader
_hang:
  b     _hang

_bss_clear:
  subs  x2, x1, x0
  b.le  _clear_complete
  str   xzr, [x0], #8
  b     _bss_clear
_clear_complete:
  ret
