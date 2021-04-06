.section ".text"
#core timer interrupt

#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global core_timer_enable
core_timer_enable:
  mov  x0, #1
  msr  cntp_ctl_el0, x0 // enable
  mrs  x0, cntfrq_el0
  lsl  x0, x0, #1
  msr  cntp_tval_el0, x0 // set expired time
  mov  x0, #2
  ldr  x1, =CORE0_TIMER_IRQ_CTRL
  str  w0, [x1] // unmask timer interrupt
  ret
.global core_timer_handler
core_timer_handler:
  mrs  x0, cntfrq_el0
  lsl  x0, x0, #1
  msr  cntp_tval_el0, x0
  sub  sp, sp, #16
  stp  lr, x0, [sp, #16 * 0]
  bl  get_core_time
  ldp  lr, x0, [sp, #16 * 0]
  add  sp, sp, #16
  ret