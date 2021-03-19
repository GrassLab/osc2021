.section ".text.startup"

.global delay
.global reset

_start:
  adr x1, DTB_ADDR
  str x0, [x1]
  adr x1, bss_begin
  adr x2, bss_end
  subs x2, x2, x1
  beq 2f
1:
  str xzr, [x1], #8
  subs x2, x2, #8
  bne 1b
2:
  mov sp, #0x00800000
  bl miniuart_init
  bl shell

Infinite_Loop:
  b Infinite_Loop

//250MHz
delay:
  subs w0, w0, #1
  bne delay
  ret

.equ PM_PASSWORD, 0x5A000000
.equ PM_RSTC, 0x3F10001C
.equ PM_WDOG, 0x3F100024

reset:
  ldr w0, =PM_PASSWORD
  orr w1, w0, #0x20  //PM_RSTC_WRCFG_FULL_RESET
  ldr x2, =PM_RSTC
  str w1, [x2]
  mov w1, #1  //delay
  lsl w1, w1, #16
  orr w1, w1, w0
  ldr x2, =PM_WDOG
  str w1, [x2]
  b Infinite_Loop

.global DTB_ADDR

.data
  DTB_ADDR: .word 0
