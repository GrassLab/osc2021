.section ".text.startup"

.global delay
.global reset

_start:
  adr x0, bss_begin
  adr x1, bss_end
  subs x1, x1, x0
  beq 2f
1:
  str xzr, [x0], #8
  subs x1, x1, #8
  bne 1b
2:
  mov sp, #0x800000
  bl miniuart_init
  bl shell

Infinite_Loop:
  b Infinite_Loop

//250MHz
delay:
  subs x0, x0, #1
  bne delay
  ret

reset:
  mov w0, #0x5A000000  //PM_PASSWORD
  orr w1, w0, #0x20  //PM_RSTC_WRCFG_FULL_RESET
  ldr w2, =0x3F10001C  //PM_RSTC
  str w1, [x2]
  mov w1, #1  //delay
  lsl w1, w1, #16
  orr w1, w1, w0
  ldr w2, =0x3F100024  //PM_WDOG
  str w1, [x2]
  b Infinite_Loop
