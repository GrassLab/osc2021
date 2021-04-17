.section ".text"

.global core_timer_enable
.global core_timer_handler

.equ CORE0_TIMER_IRQ_CTRL, 0x40000040

core_timer_enable:
  mov x0, 1
  msr cntp_ctl_el0, x0 // enable
  mrs x0, cntfrq_el0
  msr cntp_tval_el0, x0 // set expired time
  mov x0, 2
  ldr x1, =CORE0_TIMER_IRQ_CTRL
  str w0, [x1] // unmask timer interrupt
  ret

core_timer_handler:
  mrs x0, cntfrq_el0
  mrs x1, cntpct_el0
  stp lr, x0, [sp, #-16]!
  bl print_core_timer
  ldp lr, x0, [sp], #16
  lsl x0, x0, #1
  msr cntp_tval_el0, x0
  ret
