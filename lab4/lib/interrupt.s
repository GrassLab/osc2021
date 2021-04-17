.section ".text"

.global core_timer_enable

core_timer_enable:
  mov x0, 1
  msr cntp_ctl_el0, x0 // enable
  mrs x0, cntfrq_el0
  add x0, x0, x0
  msr cntp_tval_el0, x0 // set expired time
  mov x0, 2
  ldr x1, =0x40000040 
  str w0, [x1] // unmask timer interrupt
  ret

//.global core_timer_handler
//core_timer_handler:
//  mrs x0, cntfrq_el0
//  msr cntp_tval_el0, x0
