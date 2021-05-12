.section ".text.startup"

.global delay
.global reset
.global Infinite_Loop

_start:
  adr x1, DTB_ADDR
  str x0, [x1]

  mov x0, xzr
  str xzr, [x0]

  bl from_el2_to_el1
  bl set_exception_vector_table
  bl core_timer_enable

  adr x0, KERNEL_ADDR_END
  adr x1, bss_begin
  adr x2, bss_end
  str x2, [x0]
  subs x2, x2, x1
  beq 2f
1:
  str xzr, [x1], #8
  subs x2, x2, #8
  bne 1b
2:
  adr x0, _start
  mov X1, #1
  mvn x1, x1
  and x0, x0, x1
  mov sp, x0

  bl miniuart_init
  bl cpio_init
  bl buddy_init
  bl shell
  mov x0, #0

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

.global KERNEL_ADDR_END
.global INITRAMFS_ADDR
.global INITRAMFS_ADDR_END
.global DTB_ADDR

.data
  KERNEL_ADDR_END: .xword 0
  INITRAMFS_ADDR: .xword 0
  INITRAMFS_ADDR_END: .xword 0
  DTB_ADDR: .xword 0
