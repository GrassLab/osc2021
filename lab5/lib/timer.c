#include <stdint.h>
#include "sysreg.h"
#include "reg.h"
#include "printf.h"
#include "current.h"
#include "sched.h"
#include "list.h"
#include "timer.h"

void set_current_sleep(size_t t) {
    current->timer = t;
    task_pause(current);
    schedule();
}

void enable_core_timer() {
    write_sysreg(cntp_ctl_el0, 1LL);

    unsigned long frq = read_sysreg(cntfrq_el0);

    if (frq != CPU_HZ) {
        panic("[Kernel] CPU frequency unmatch")
    }
    /* set timer ~= 1/65536 second */
    write_sysreg(cntp_tval_el0, CPU_HZ / TICK_PER_INT);

    /* enable rpi3 timer interrupt */
    *CORE0_TIMER_IRQ_CTRL = 2;
}

/* we should ensure current got assigned before timer handler got triggered */
void core_timer_handler() {
    write_sysreg(cntp_tval_el0, CPU_HZ / TICK_PER_INT);

    unsigned long tick = read_sysreg(cntpct_el0);
    //size_t sec = tick / CPU_HZ;
    //size_t r = tick % CPU_HZ;
    //size_t sec_1 = (r*10) / CPU_HZ;

    //printf("[%d.%d] Timer interrupt\n\r", sec, sec_1);

    struct task_struct *cur = current;
    unsigned long elapsed_tick = tick - cur->last_tick;

    if (elapsed_tick >= cur->remained_tick) {
        cur->remained_tick = 0;
        schedule();
    } else {
        cur->remained_tick -= elapsed_tick;
        cur->last_tick = tick;
    }
}