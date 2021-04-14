#include <stdint.h>
#include "sysreg.h"
#include "printf.h"

#define __REG_TYPE volatile uint32_t
#define __REG __REG_TYPE *
#define CORE0_TIMER_IRQ_CTRL ((__REG) 0x40000040)

void enable_core_timer() {
    write_sysreg(cntp_ctl_el0, 1);

    unsigned long frq = read_sysreg(cntfrq_el0);
    /* set timer = 2 second */
    write_sysreg(cntp_tval_el0, frq * 2);

    /* enable rpi3 timer interrupt */
    *CORE0_TIMER_IRQ_CTRL = 2;
}

void core_timer_handler() {
    unsigned long frq = read_sysreg(cntfrq_el0);
    write_sysreg(cntp_tval_el0, frq * 2);

    unsigned long cnt = read_sysreg(cntpct_el0);
    printf("[%d.000000] Timer interrupt\n", cnt/frq);
}