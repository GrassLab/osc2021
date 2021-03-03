.section ".text"

.globl _start
_start:
  // Set bss as zero
  // TODO
  ldr   x0, =__bss_begin
  ldr   x1, =__bss_end
  bl    _bss_clear
  // Set stack pointer to #LOW_MEMORY (0xffffffff)
  ldr   x0, =0xffffffff
  mov   sp, x0
  // Jump to kernel
  bl    kernel
_hang:
  b     _hang

_bss_clear:
  sub   x2, x1, x0
  cbz   x2, _clear_complete
  str   xzr, [x0], #8
  b     _bss_clear
_clear_complete:
  ret
