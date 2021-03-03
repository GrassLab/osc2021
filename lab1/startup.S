.global _start
.section ".text"

_start:
  ldr x20, =_sbss
  ldr x21, =_ebss

  /* setup interrupt vector base */
  ldr x0, =_svector
  msr VBAR_EL2, x0

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

.section ".vector"
.balign 0x800
curr_el_sp0_sync:
  wfe

.balign 0x80
curr_el_sp0_irq:
  wfe

.balign 0x80
curr_el_sp0_fiq:
  wfe

.balign 0x80
curr_el_sp0_serror:
  wfe

.balign 0x80
curr_el_spx_sync:
  wfe

.balign 0x80
curr_el_spx_irq:
  wfe

.balign 0x80
curr_el_spx_fiq:
  wfe

.balign 0x80
curr_el_spx_serror:
  wfe

.balign 0x80
lower_el_aarch64_sync:
  wfe

.balign 0x80
lower_el_aarch64_irq:
  wfe

.balign 0x80
lower_el_aarch64_fiq:
  wfe

.balign 0x80
lower_el_aarch64_serror:
  wfe

.balign 0x80
lower_el_aarch32_sync:
  wfe

.balign 0x80
lower_el_aarch32_irq:
  wfe

.balign 0x80
lower_el_aarch32_fiq:
  wfe

.balign 0x80
lower_el_aarch32_serror:
  wfe
