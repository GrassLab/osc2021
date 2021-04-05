
.globl core_timer_handler
core_timer_handler:
    ldr   x0, =0x927C000  // 19.2MHz
    msr   cntp_tval_el0, x0
    ret          

.globl core_timer_get_sec
core_timer_get_sec:
	mrs   x2, cntfrq_el0
	mrs   x1, cntpct_el0
	udiv  x0, x1, x2
	ret