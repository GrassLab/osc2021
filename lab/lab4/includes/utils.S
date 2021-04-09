.global _moveTo
_moveTo:
  add sp, sp, x1
  br x0

.global _branch
_branch:
  br x0


.global _run_el0
_run_el0:
    mov x2, 0x3c0
    msr spsr_el1, x2 // interupt set up
    msr sp_el0, x1
    msr elr_el1, x0
    eret // return to EL0

.global _get_el
_get_el:
    mrs x0, CurrentEL
    lsr x0, x0, #2
    ret

#define CORE0_TIMER_IRQ_CTRL 0x40000040
.global _core_timer_enable
_core_timer_enable:
		msr cntp_tval_el0, x0 // set expired time
		mov x0, 1
		msr cntp_ctl_el0, x0 // enable
		; mrs x0, cntfrq_el0
		; mov x0, 2
		mov x0, 2
		ldr x1, =CORE0_TIMER_IRQ_CTRL
		str w0, [x1] // unmask timer interrupt
    ret

.globl _core_timer_disable
_core_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0
    mov x0, 0
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1]          // disable timer interrupt
    ret
    
_core_timer_handler:
		mrs x0, cntfrq_el0
		msr cntp_tval_el0, x0