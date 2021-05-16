#include <stdint.h>
#include "sysreg.h"
#include "reg.h"
#include "printf.h"
#include "current.h"
#include "sched.h"
#include "list.h"
#include "timer.h"
#include "mm.h"

size_t jiffies;
struct ktimer *timer_list;

/* no need to mask since we don't permit nested IRQ */
static inline void jiffies_inc() {
    jiffies += 1;
}

size_t get_jiffies() {
    return jiffies;
}

void add_timer(struct ktimer *timer) {
    timer->next = timer_list;
    timer_list = timer;
}

void timeout(void (*fn)(size_t), size_t arg, size_t msec) {
    struct ktimer *timer = kmalloc(sizeof(struct ktimer));
    timer->fn = fn;
    timer->arg = arg;
    timer->timeout_tick = get_jiffies() + MS(msec);
    timer->need_gc = 0;
    add_timer(timer);
}

void task_sleep(size_t msec) {
    disable_interrupt();
    timeout((timeout_cb)restart_task, (size_t)current, msec);
    pause_task(current);

    schedule();
    enable_interrupt();
}

void run_timers() {
    struct ktimer **p = &timer_list;
    struct ktimer *tp;

    while (*p != NULL) {
        tp = *p;

        if (tp->need_gc) {
            while (*p != NULL && tp->need_gc) {
                struct ktimer *nxt = tp->next;
                kfree(tp);
                *p = nxt;
                tp = *p;
            }
            continue;
        }

        if (get_jiffies() >= tp->timeout_tick) {
            tp->fn(tp->arg);
            tp->need_gc = 1;
        }
        p = &(*p)->next;
    }
}

void enable_core_timer() {
    write_sysreg(cntp_ctl_el0, 1LL);

    unsigned long frq = read_sysreg(cntfrq_el0);
    printf("[Kernel] CPU freq: %ld\n\r", frq);

    if (frq != CPU_HZ) {
        panic("timer: CPU frequency unmatch")
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