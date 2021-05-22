#include <current.h>
#include <sched.h>
#include <context_switch.h>
#include <mm.h>
#include <string.h>
#include <interrupt.h>
#include <sched.h>

/* for userland */
static struct task_struct *fork_context() {
    struct task_struct *ts = kmalloc(sizeof(struct task_struct));

    size_t flags = disable_irq_save();

    *ts = *current;

    ts->need_resched = 0;
    ts->preempt_count = 0;

    struct pt_regs *old_trapframe = (struct pt_regs *)((size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));

    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->stack = kmalloc(USTACK_SIZE);
    memcpy(ts->stack, current->stack, USTACK_SIZE);

    struct pt_regs *trapframe = (struct pt_regs *)((size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memcpy(trapframe, old_trapframe, sizeof(struct pt_regs));

    /* TODO: enable mmu to get rid of this shit */
    trapframe->sp = (size_t)ts->stack + (old_trapframe->sp - (size_t)current->stack);
    trapframe->regs[0] = 0;

    irq_restore(flags);

    ts->pid = get_next_pid();

    /* don't duplicate user program memory,
     * since in this lab we only put it into stop queue, we won't encounter UAF
     * TODO: enable MMU to get rid off this shit */
    ts->user_prog = NULL;

    ts->cpu_context.sp = (size_t)trapframe;
    ts->cpu_context.pc = (size_t)restore_regs_eret;

    return ts;
}

/* we use fork() to get all context, so trapframe will be construct first
 * so be careful to use this function only for fork syscall */
static pid_t fork_user_thread() {
    struct task_struct *ts = fork_context();
    add_task(ts);

    return ts->pid;
}

pid_t do_fork() {
    return fork_user_thread();
}