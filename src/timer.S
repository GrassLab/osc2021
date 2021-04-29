#include "config.h"
#define CORE0_TIMER_IRQ_CTRL 0x40000040

.global core_timer_enable
core_timer_enable:
    mov    x0, 1
    msr    cntp_ctl_el0, x0
    mrs    x0, cntfrq_el0
    msr    cntp_tval_el0, x0

    #ifdef TIMER_INTERRUPT_ENABLE
    mov    x0, 2
    #else
    mov    x0, 0
    #endif

    ldr    x1, =CORE0_TIMER_IRQ_CTRL
    str    w0, [x1]
    ret
