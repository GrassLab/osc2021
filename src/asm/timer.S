#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global core_timer_enable
core_timer_enable:
    mov x0, 1
    msr cntp_ctl_el0, x0            // enable

    mrs x0, cntfrq_el0
    mov x1, 5
    mul x0, x0, x1
    msr cntp_tval_el0, x0           // set expired time
    
    mov x0, 2
    ldr x1, =CORE0_TIMER_IRQ_CTRL
    str w0, [x1]                    // unmask timer 
    ret

.global core_timer_disable
core_timer_disable:
    mrs x0, cntp_ctl_el0
    mov x1, 1
    eor x1, x1, x1                  // not x1
    and x0, x0, x1
    msr cntp_ctl_el0, x0

    ldr x2, =CORE0_TIMER_IRQ_CTRL
    ldr x0, [x2]
    mov x1, 2
    eor x1, x1, x1                  // not x1
    and x0, x0, x1
    str x0, [x2]                    // enable timer interrupt
    ret