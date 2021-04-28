/*
	core_timer_enable
	
	cntp_ctl_el0: Physical Timer Control register.
	              1: enable, 0: disable
	
	cntpct_el0: The timer’s current count
	cntp_cval_el0: A compared timer count. If cntpct_el0 >= cntp_cval_el0, interrupt the CPU core.
	cntp_tval_el0: (cntp_cval_el0 - cntpct_el0). You can use it to set an expired timer after the current timer count.
	cntfrq_el0: the frequency of timer
	
	1. set cntp_ctl_el0 enable
	2. set cntp_tval_el0 is cntfrq_el0 (i.e. expired time is cntfrq_el0)
	3. unmask the timer interrupt from the first level interrupt controller
*/

.equ CORE0_TIMER_IRQ_CTRL, 0x40000040

.globl core_timer_enable
core_timer_enable:
	mov x0, 1
	msr cntp_ctl_el0, x0 // enable
	mrs x0, cntfrq_el0
	msr cntp_tval_el0, x0 // set expired time
	mov x0, 2
	ldr x1, =CORE0_TIMER_IRQ_CTRL
	str w0, [x1] // unmask timer interrupt
	ret