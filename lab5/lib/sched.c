#include "list.h"
#include "sched.h"
#include "sysreg.h"
#include "mm.h"
#include "context_switch.h"
#include "current.h"
#include "string.h"
#include "timer.h"
#include "printf.h"

size_t GLB_PID;
struct list_head run_queue = LIST_HEAD_INIT(run_queue);
/* slept process get placed to paused queue */
struct list_head paused_queue = LIST_HEAD_INIT(paused_queue);
struct list_head stopped_queue = LIST_HEAD_INIT(stopped_queue);

/* should be spawned by kernel in order to manage slept process */
// void timer_daemon() {

// }


inline void del_task(struct task_struct *ts) {
    unlink(&ts->list);
}

inline void add_task(struct task_struct *ts) {
    insert_tail(&run_queue, &ts->list);
}

void task_pause(struct task_struct *ts) {
    ts->state = TASK_PAUSED;
    del_task(current);

    insert_head(&paused_queue, &ts->list);
}

/* we don't need to make trapframe for kthread,
 * since it will be auto created if the kthread eret to userland */
/* can we set function arguments (e.g. rdi, rsi) ? */
void schedule_kthread(void *cb) {
    struct task_struct *ts = kcalloc(sizeof(struct task_struct));
    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->state = TASK_RUNNING;
    ts->pid = GLB_PID++;
    ts->need_sched = 0;
    ts->remained_tick = TICK_PER_INT * 10;

    /* reserve space for pt_regs on kstack to make in-place exec doable */
    ts->cpu_context.sp = (size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs);
    ts->cpu_context.pc = (size_t)cb;

    add_task(ts);
}

static struct task_struct *pick_next_task() {
    if (list_empty(&run_queue)) {
        panic("[Kernel] Scheduler: run queue is empty");
    }
    struct task_struct *ts = list_first_entry(&run_queue, struct task_struct, list);
    del_task(ts);
    insert_tail(&run_queue, &ts->list);

    return ts;
}

void wake_up_process(struct task_struct *ts) {

}

void kill_task(struct task_struct *target, int status) {
    del_task(target);
    target->state = TASK_STOPPED;
    target->exitcode = status;
    insert_head(&stopped_queue, &target->list);
}

static void switch_task(struct task_struct *nxt) {
    switch_to(&current->cpu_context, &nxt->cpu_context);
}

/* allocate a empty struct to save init process */
/* the first task switch may be switch_to(init, init),
 * but it doesn't matter */
void set_init_thread() {
    struct task_struct *cur = kcalloc(sizeof(struct task_struct));
    cur->remained_tick = 0;
    cur->state = TASK_RUNNING;
    set_current(cur);

    add_task(cur);
}

/* calc & select next task to run */
void schedule() {
    struct task_struct *nxt = pick_next_task();
    nxt->last_tick = read_sysreg(cntpct_el0);
    nxt->remained_tick = TICK_PER_INT * 5;
    switch_task(nxt);
}