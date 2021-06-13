#ifndef __TIMER_H_
#define __TIMER_H_

#ifdef RPI3
#define CPU_HZ 19200000LL
#else
#define CPU_HZ 62500000LL
#endif

#define TIMER_HZ 1000LL
#define MSEC_PER_SEC 1000LL
#define MSEC_PER_TICK (MSEC_PER_SEC / TIMER_HZ)
#define TICK_PER_INT (CPU_HZ / TIMER_HZ)
#define MS(n) (n / MSEC_PER_TICK)

typedef void (*timeout_cb)(size_t);
/* TODO: find a way to impl `stop_timer()`
 * we may need to know all the reference to timer */
struct ktimer {
    struct ktimer *next;
	unsigned long timeout_tick;
    timeout_cb fn;
	size_t arg;
    char need_gc;
};

void enable_core_timer();
void core_timer_handler();
size_t get_jiffies();
void add_timer(struct ktimer *);
void run_timers();
void timeout(void (*fn)(size_t), size_t arg, size_t msec);
void task_sleep(size_t msec);

#endif