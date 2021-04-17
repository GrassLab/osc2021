
.macro print_el registers
	sub sp, sp, 32
	str lr, [sp]
	mov x0, \registers
    bl uart_printhex
	ldr lr, [sp]
	add sp, sp, 32
.endm

#define CORE0_TIMER_IRQ_CTRL 0x40000040
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

.globl core_timer_disable
core_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0
    mov x0, 0
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str x0, [x1]          // disable timer interrupt
    ret

.globl el0_timer_handler
el0_timer_handler:
    mrs x0, cntpct_el0
    print_el x0
    mrs x0, cntfrq_el0
    add x0, x0, x0       // 2 seconds
    msr cntp_tval_el0, x0
    eret

.globl get_current_timer_cnt
get_current_timer_cnt:
    mrs x0, cntpct_el0
    ret

.globl get_timer_freq
get_timer_freq:
    mrs x0, cntfrq_el0
    ret

