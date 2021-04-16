#include "list.h"
#include "sched.h"
extern struct task_struct *__current_entry[];

struct list_head sched_task = LIST_HEAD_INIT(sched_task);

/* calc & select next task to run */
void schedule_task(struct task_struct *ts) {

}

void run_task(struct task_struct *ts) {
    /* no task switching, only run single task */
    __current_entry[0] = ts;
    asm("msr elr_el1, %0\n\t"
        "msr spsr_el1, xzr\n\t"
        "msr sp_el0, %1\n\t"
        "mov sp, %2\n\t"
        "eret\n\t"
        :: "r" (ts->user_prog),
           "r" (ts->stack + USTACK_SIZE),
           "r" (ts->kstack + KSTACK_SIZE));
}