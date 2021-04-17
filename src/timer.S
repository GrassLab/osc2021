#define CORE0_TIMER_IRQ_CTRL 0x40000040
#define TIMER_EN        (1<<0)
#define TIMER_INTEN     (1<<1)

// Core Timer Interrupt(CORE0_TIMER_IRQ_CTRL), 
// See QA7_rev3.7 data sheet - Core timers interrupt for more details
// bit[0]: nCNTPSIRQ:    secure physical timer
// bit[1]: nCNTPNSIRQ:   Non-secure physical timer
// bit[2]: nCNTHPIRQ:    Hypervisor Timer Event
// bit[3]: nCNTVIRQ:     Virtual Timer Event
// bit[4]: nCNTPSIRQ:    FIQ
// bit[5]: nCNTPNSIRQ:   FIQ
// bit[6]: nCNTHPIRQ:    FIQ
// bit[7]: nCNTVIRQ:     FIQ
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
    str w0, [x1]                    // unmask timer intterupt for the  first level interrupt controller
    ret

.global core_timer_disable
core_timer_disable:
    mrs x0, cntp_ctl_el0
    ldr x1, =TIMER_EN
    eor x1, x1, x1 // logical xor -> for not x1
    and x0, x0, x1
    msr cntp_ctl_el0, x0

    ldr x2, =CORE0_TIMER_IRQ_CTRL
    ldr x0, [x2]
    ldr x1, =TIMER_INTEN
    eor x1, x1, x1 // logical xor -> for not x1
    and x0, x0, x1
    str x0, [x2] // disable timer interrupt
    ret

.global core_timer_handler
core_timer_handler:
    mrs x0, cntfrq_el0
    mov x1, 5
    mul x0, x0, x1
    msr cntp_tval_el0, x0
    ret



