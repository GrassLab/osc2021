#include <stdint.h>
#include "sysreg.h"
#include "reg.h"
#include "printf.h"
#include "current.h"
#include "sched.h"
#include "list.h"
#include "timer.h"

size_t jiffies = 0;

static inline void jiffies_inc() {
    jiffies += 1;
}

size_t get_jiffies() {
    return jiffies;
}

/* argument use ms as unit */
void set_current_sleep(size_t t) {
    current->timer = MS(t);
    pause_task(current);
    schedule();
}

void enable_core_timer() {
    write_sysreg(cntp_ctl_el0, 1LL);

    unsigned long frq = read_sysreg(cntfrq_el0);
    printf("[Kernel] CPU freq: %ld\n\r", frq);

    if (frq != CPU_HZ) {
        panic("[Kernel] CPU frequency unmatch")
    }

    write_sysreg(cntp_tval_el0, CPU_HZ / TIMER_HZ);

    /* enable rpi3 timer interrupt */
    *CORE0_TIMER_IRQ_CTRL = 2;
}

/* we should ensure current got assigned before timer handler got triggered */
void core_timer_handler() {
    jiffies_inc();
    write_sysreg(cntp_tval_el0, CPU_HZ / TIMER_HZ);

    //unsigned long tick = read_sysreg(cntpct_el0);
    //size_t sec = tick / CPU_HZ;
    //size_t r = tick % CPU_HZ;
    //size_t sec_1 = (r*10) / CPU_HZ;

    //printf("[%d.%d] Timer interrupt\n\r", sec, sec_1);

    struct task_struct *cur = current;

    if (jiffies >= cur->timeout_tick) {
        cur->need_resched = 1;
    }
}