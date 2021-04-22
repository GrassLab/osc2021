#define CORE0_TIMER_IRQCNTL 0x40000040

.global core_timer_enable
.global core_timer_disable

core_timer_enable:
    // set expired time to 2 seconds according to the 
    // frequency of the system timer CNTFRQ_EL0
    mrs x0, CNTFRQ_EL0
    add x0, x0, x0
    msr CNTP_TVAL_EL0, x0

    // unmask timer interrupt
    mov w0, 2
    ldr x1, = CORE0_TIMER_IRQCNTL
    str w0, [x1]

    // enable
    mov x0, 1
    msr CNTP_CTL_EL0, x0 

    ret

core_timer_disable:
    // mask timer interrupt
    ldr x1, = CORE0_TIMER_IRQCNTL
    str w0, [x1]

    // disable
    mov x0, 0
    msr CNTP_CTL_EL0, x0 

    ret
