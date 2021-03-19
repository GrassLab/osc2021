.global _start
.section ".text"

_start:
  ldr x1, =_dtb_ptr
  str x0, [x1]
  ldr x20, =_sbss
  ldr x21, =_ebss

  /* setup interrupt vector base */
  //ldr x0, =_svector
  //msr VBAR_EL2, x0

bss_loop:
  cmp x20, x21
  b.eq bss_loop_end
  str xzr, [x20]
  add x20, x20, #8
  b bss_loop
bss_loop_end:

  ldr x20, =_estack
  mov sp, x20

  bl main
  wfe

.global _dtb_ptr
.section .data
_dtb_ptr: .dc.a 0x0