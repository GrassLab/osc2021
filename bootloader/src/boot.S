.section ".text.boot"

.globl _start
_start:
  // disable multi core
  mrs   x0, mpidr_el1
  and   x0, x0, #3
  cbnz  x0, _hang
  // self relocation
  ldr   x0, =0x60000
  ldr   x1, =0x80000
  bl    _relocate
  //mov   x0, pc
  //ldr   x0, =0x60020
  b     #-0x20000+4
  // Set stack pointer before code session
  ldr   x0, =_start
  //sub   x0, x0, #0x20000
  mov   sp, x0
  // Set bss as zero
  ldr   x0, =__bss_begin
  //sub   x0, x0, #0x20000
  ldr   x1, =__bss_end
  //sub   x1, x1, #0x20000
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

_relocate:
  cmp   x0, #0x64000
  b.ge  _relocate_complete
  ldr   x2, [x1], #8
  str   x2, [x0], #8
  b     _relocate
_relocate_complete:
  ret
