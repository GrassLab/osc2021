#ifndef __PREEMPT_H_
#define __PREEMPT_H_
#include <current.h>
#include <sched.h>
#include <atomic.h>

#define disable_preempt() preempt_count_inc()
#define enable_preempt() preempt_count_dec()

static inline void preempt_count_inc() {
    atomic_add_fetch(current->preempt_count, 1);
}

static inline void preempt_count_dec() {
    atomic_sub_fetch(current->preempt_count, 1);
}

#endif