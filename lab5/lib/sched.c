#include <list.h>
#include <sched.h>
#include <sysreg.h>
#include <mm.h>
#include <context_switch.h>
#include <current.h>
#include <string.h>
#include <timer.h>
#include <printf.h>
#include <interrupt.h>
#include <timer.h>
#include <preempt.h>
#include <atomic.h>

#define DEFAULT_TIMEOUT 5

pid_t GLB_PID;
struct list_head run_queue = LIST_HEAD_INIT(run_queue);
/* slept process get placed to paused queue */
struct list_head paused_queue = LIST_HEAD_INIT(paused_queue);
struct list_head stopped_queue = LIST_HEAD_INIT(stopped_queue);

pid_t get_next_pid() {
    return atomic_fetch_add(GLB_PID, 1);
}

/* FUCK YEAH we have to fix all these shit to disable_irq_save
 * to prevent race with timer (sleep callback)
 */
inline void del_task(struct task_struct *ts) {
    size_t flags = disable_irq_save();
    unlink(&ts->list);
    irq_restore(flags);
}

inline void add_task(struct task_struct *ts) {
    size_t flags = disable_irq_save();
    insert_tail(&run_queue, &ts->list);
    irq_restore(flags);
}

void pause_task(struct task_struct *ts) {
    size_t flags = disable_irq_save();

    if (ts->state != TASK_RUNNING) {
        panic("task state inconsistent");
    }
    ts->state = TASK_PAUSED;
    ts->need_resched = 1; /* TODO: fix this shit */
    unlink(&ts->list);

    insert_head(&paused_queue, &ts->list);

    irq_restore(flags);
}

void restart_task(struct task_struct *ts) {
    size_t flags = disable_irq_save();

    if (ts->state != TASK_PAUSED) {
        panic("task state inconsistent");
    }
    ts->state = TASK_RUNNING;
    unlink(&ts->list);

    insert_head(&run_queue, &ts->list);

    irq_restore(flags);
}

void kill_task(struct task_struct *ts, int status) {
    size_t flags = disable_irq_save();

    ts->state = TASK_STOPPED;
    ts->need_resched = 1;
    unlink(&ts->list);

    ts->exitcode = status;
    insert_head(&stopped_queue, &ts->list);

    irq_restore(flags);
}

void free_task(struct task_struct *ts) {
    kfree(ts->stack);
    kfree(ts->kstack);
    kfree(ts->user_prog);
    kfree(ts);
}

/* we don't need to make trapframe for kthread,
 * since it will be auto created if the kthread eret to userland */
/* can we set function arguments (e.g. rdi, rsi) ? */
void schedule_kthread(void *cb) {
    struct task_struct *ts = kcalloc(sizeof(struct task_struct));
    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->state = TASK_RUNNING;
    ts->pid = get_next_pid();
    ts->timeout_tick = MS(DEFAULT_TIMEOUT);

    /* reserve space for pt_regs on kstack to make in-place exec doable */
    ts->cpu_context.sp = (size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs);
    ts->cpu_context.pc = (size_t)cb;

    add_task(ts);
}

static struct task_struct *pick_next_task() {
    if (list_empty(&run_queue)) {
        panic("scheduler: run queue is empty");
    }
    struct task_struct *ts = list_first_entry(&run_queue, struct task_struct, list);
    unlink(&ts->list);
    insert_tail(&run_queue, &ts->list);

    return ts;
}

static void switch_task(struct task_struct *nxt) {
    switch_to(&current->cpu_context, &nxt->cpu_context);
}

/* allocate a empty struct to save init process */
/* the first task switch may be switch_to(init, init),
 * but it doesn't matter */
void set_init_thread() {
    struct task_struct *cur = kcalloc(sizeof(struct task_struct));
    cur->state = TASK_RUNNING;
    cur->pid = get_next_pid();
    set_current(cur);

    add_task(cur);
}

void schedule() {
    if (!current->need_resched || current->preempt_count > 0) {
        return;
    }

    size_t flags = disable_irq_save();

    struct task_struct *nxt = pick_next_task();
    nxt->timeout_tick = get_jiffies() + MS(DEFAULT_TIMEOUT);
    nxt->need_resched = 0;

    /* task get switch out & return from here */
    switch_task(nxt);

    irq_restore(flags);
}