#include <current.h>
#include <sched.h>
#include <context_switch.h>
#include <mm.h>
#include <string.h>
#include <interrupt.h>
#include <sched.h>
#include <syscall_wrapper.h>
#include <mmu.h>
#include <printf.h>

static void fork_vm(pd_t *tbl, struct vm_area *vmlist) {
    struct vm_area *vm = vmlist;

    while (vm) {
        for (int i = 0; i < vm->size; i += PAGE_SIZE) {
            void *p = kcalloc(PAGE_SIZE);
            map_user_page(tbl, vm->va + i, virt_to_phys(p), vm->flags);

            size_t entry = get_PTE(vm->va + i);
            /* fail bit */
            if (entry & 1) {
                panic("failed to map virtual address (0x%lx)", vm->va + i);
            }

            void *old_p = phys_to_virt(entry & PAR_PA_MASK);
            memcpy(p, old_p, PAGE_SIZE);
        }

        vm = vm->next;
    }
}

/* for userland */
static struct task_struct *fork_context() {
    struct task_struct *ts = kmalloc(sizeof(struct task_struct));

    size_t flags = disable_irq_save();

    *ts = *current;

    ts->need_resched = 0;
    ts->preempt_count = 0;

    struct pt_regs *old_trapframe = (struct pt_regs *)((size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));

    ts->ttbr0 = kcalloc(PAGE_SIZE);
    ts->kstack = kmalloc(KSTACK_SIZE);
    ts->stack = alloc_user_stack(ts->ttbr0, USTACK_SIZE);
    memcpy(ts->stack, current->stack, USTACK_SIZE);

    fork_vm(ts->ttbr0, ts->vm_map);

    struct pt_regs *trapframe = (struct pt_regs *)((size_t)ts->kstack + KSTACK_SIZE - sizeof(struct pt_regs));
    memcpy(trapframe, old_trapframe, sizeof(struct pt_regs));

    trapframe->regs[0] = 0;

    irq_restore(flags);

    ts->pid = get_next_pid();
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

SYSCALL_DEFINE0(fork) {
    return fork_user_thread();
}