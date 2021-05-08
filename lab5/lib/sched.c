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

/* should be spawned by kernel in order to manage slept process */
// void timer_daemon() {

// }

static uint32_t align_up(uint32_t size, int alignment) {
  return (size + alignment - 1) & -alignment;
}

static void *init_stack_args(char *stack, const char *argv[]) {
    const char *s;
    unsigned argc = 0;
    unsigned s_size = 0;
    unsigned a_size;
    while (s = argv[argc++]) {
        /* TODO: validate pointer */
        s_size += strlen(s) + 1;
    }

    s_size = align_up(s_size, 16);
    /* total 1(argc) + argc+1(argv+NULL) */
    a_size = align_up((argc + 2) * sizeof(size_t), 16);
    size_t *top = (size_t *)(stack - (s_size + a_size));
    char *s_ptr = (char *)top + a_size;

    top[0] = argc;
    for (int i = 0; i < argc; i++) {
        top[i+1] = (size_t)s_ptr;
        unsigned n = strlen(argv[i]) + 1;
        memcpy(s_ptr, argv[i], n);
        s_ptr += n;
    }
    top[argc+1] = 0;

    return top;
}

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

asm("eret: eret");
void eret();

struct task_struct *alloc_user_task(void *prog, const char *argv[]) {
    struct task_struct *ts = kmalloc(sizeof(struct task_struct));

    /* TODO: protect page with virtual memory */
    ts->stack = kcalloc(USTACK_SIZE);
    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->pid = GLB_PID++;
    ts->user_prog = prog;

    char *stack_top = init_stack_args(ts->stack + USTACK_SIZE, argv);
    if (!stack_top) {
        return NULL;
    }

    struct pt_regs *trapframe = (struct pt_regs *)((char *)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));

    /* TODO: parse ELF to get entrypoint */
    trapframe->pc = (size_t)prog;
    trapframe->sp = (size_t)stack_top;

    ts->cpu_context.pc = (size_t)&eret;
    ts->cpu_context.sp = (size_t)trapframe;

    add_task(ts);

    return ts;
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

    ts->cpu_context.sp = (size_t)ts->kstack + KSTACK_SIZE;
    ts->cpu_context.pc = (size_t)cb;

    add_task(ts);
}

/* we copy all the registers/stack context, but not kstack context
 * to new task_struct, so it's easier to differentiate forked process
 * and the original */
/* we need to construct trapframe ourself, or we just set pc to eret ? */
struct task_struct *fork_context() {
    struct task_struct *cur = kmalloc(sizeof(struct task_struct));
    cur->kstack = kmalloc(KSTACK_SIZE);

    return NULL;
}

/* we use fork() to get all context, so trapframe will be construct first
 * so be careful to use this function only for fork syscall */
void schedule_user_thread(size_t cb) {
    struct task_struct *ts = fork_context();

    /* TODO: protect page with virtual memory */
    /* TODO: use schedule_task() to do scheduling */
    ts->stack = kmalloc(USTACK_SIZE);
    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->pid = GLB_PID++;

    ts->cpu_context.sp = (size_t)ts->kstack + KSTACK_SIZE;
    ts->cpu_context.pc = (size_t)cb;

    add_task(ts);
}

struct task_struct *pick_next_task() {
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

void switch_task(struct task_struct *nxt) {
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

void run_task(struct task_struct *ts) {
    /* no task switching, only run single task */
    set_current(ts);

    asm("msr elr_el1, %0\n\t"
        "msr spsr_el1, xzr\n\t"
        "msr sp_el0, %1\n\t"
        "mov sp, %2\n\t"
        "eret\n\t"
        :: "r" (ts->user_prog),
           "r" (ts->stack + USTACK_SIZE),
           "r" (ts->kstack + KSTACK_SIZE));
}