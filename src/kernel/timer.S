#define CORE0_TIMER_IRQCNTL 0x40000040
.global core_timer_enable
.global core_timer_disable
.global core_timer_reset

core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0 // enable
    mrs x0, cntfrq_el0
    add x0, x0, x0
    msr cntp_tval_el0, x0 // set expired time
    mov x0, 2
    ldr x1, = CORE0_TIMER_IRQCNTL
    str w0, [x1] // unmask timer interrupt
    ret

core_timer_disable:
    mov x0, 0
    msr cntp_ctl_el0, x0

    ldr x1, = CORE0_TIMER_IRQCNTL
    str w0, [x1] // unmask timer interrupt

core_timer_reset:
    mrs x0, cntpct_el0
    mrs x1, cntfrq_el0
    add x0, x0, x1
    msr cntp_tval_el0, x0 // set expired time
    ret