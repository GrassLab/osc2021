#include "include/mm.h"
#include "include/entry.h"
#include "include/csched.h"
#include "include/mini_uart.h"
#include "include/cirq.h"
#include "utils.h"
#include "include/vfs.h"
#include "include/syslib.h"

struct task task_pool[MAX_TASK_NR];
struct task *readyQueue;
struct task *current = 0;


struct wait_h wait_h_pool[MAX_WAIT_NR];
struct wait_args wait_args_pool[MAX_WAIT_ARGS_NR];


void set_ksp(char* sp_el1)
{
    current->ksp = sp_el1;
}

char *get_ksp()
{
    return current->ksp;
}

void sigtramp()
{
    uart_send_string("From sigtramp\r\n");
    sigreturn(0);
}

void chk_sigpend()
{
    unsigned long signal_pending, user_handler;
    struct sig_struct *cur_sigs;
    char *sp;
    // uart_send_string("From chk_sigpend\r\n");

    cur_sigs = &(current->sig);

    signal_pending = cur_sigs->sigpend;
    // For easy, handle only one kind signal.
    if (signal_pending & SIG_INT_MASK) {
        if ((user_handler = cur_sigs->user_handler[SIG_INT_NUM])) {
            cur_sigs->sigaltstack = kmalloc(0x1000);
            sp = cur_sigs->sigaltstack + 0x1000;
            run_sighand(user_handler, sp, (unsigned long)sigtramp);
        } else { // default handler
            uart_send_string("From chk_sigpend: SIGINT default handling.\r\n");
            sys_exit();
        }
    }
    else if (signal_pending & SIG_KILL_MASK) {
        if ((user_handler = cur_sigs->user_handler[SIG_KILL_NUM])) {
            cur_sigs->sigaltstack = kmalloc(0x1000);
            sp = cur_sigs->sigaltstack + 0x1000;
            run_sighand(user_handler, sp, (unsigned long)sigtramp);
        } else { // default handler
            uart_send_string("From chk_sigpend: SIGKILL default handling.\r\n");
            sys_exit();
        }
    }
    // If user defined handler matches, then it won't come to here.
    return;
}

void init_wait_pool()
{
    for (int i = 0; i < MAX_WAIT_NR; ++i)
        wait_h_pool[i].free = 1;
    for (int i = 0; i < MAX_WAIT_ARGS_NR; ++i)
        wait_args_pool[i].free = 1;
}

struct wait_h *new_wait()
{
    for (int i = 0; i < MAX_WAIT_NR; ++i) {
        if (wait_h_pool[i].free) {
            wait_h_pool[i].free = 0;
            wait_h_pool[i].head = 0;
            return &wait_h_pool[i];
        }
    }
    return 0; // NULL
}

struct wait_args *new_wait_args()
{
    for (int i = 0; i < MAX_WAIT_NR; ++i) {
        if (wait_args_pool[i].free) {
            wait_args_pool[i].free = 0;
            return &wait_args_pool[i];
        }
    }
    return 0; // NULL
}

struct wait_h *sleepQueue = 0;

void init_sleepQueue()
{
    sleepQueue = new_wait();
}


int add_to_waitQueue(struct task *new, struct wait_h *waitQueue)
{ // Add new to waitQueue's tail.
    new->status = TASK_BLOCK;
    if (!waitQueue->head) {
        waitQueue->head = new;
        new->next = new;
        new->prev = new;
        return 0;
    }
    new->next = waitQueue->head;
    new->prev = waitQueue->head->prev;
    waitQueue->head->prev->next = new;
    waitQueue->head->prev = new;

    return 0;
}

int rm_from_queue(void* args)
{
    struct wait_args *wa = (struct wait_args*)args;
    struct task *old = wa->old;
    struct wait_h *waitQueue = wa->waitQueue;

    if (old->next == old) { // old is the only one
        waitQueue->head = 0; // NULL
        old->next = 0;
        old->prev = 0;
        add_to_ready(old);
        wa->free = 1; // release struct wait_args
        return 0;
    }
    old->prev->next = old->next;
    old->next->prev = old->prev;
    if (old == waitQueue->head)
        waitQueue->head = old->next;
    old->next = 0;
    old->prev = 0;
    add_to_ready(old);
    wa->free = 1; // release struct wait_args
    return 0;
}

int sleep(int seconds)
{
    struct wait_args *wa;

    wa = new_wait_args();
    wa->old = current;
    wa->waitQueue = sleepQueue;

    rm_from_ready(current);
    add_to_waitQueue(current, sleepQueue);
    tqe_add(seconds * TICKS_FOR_ITR, (void * (*)(void *))rm_from_queue, (void*)wa);
    schedule();

    return 0;
}

void init_ts_pool()
{
    for (int i = 0; i < MAX_TASK_NR; ++i)
        task_pool[i].free = 1;
}

extern struct mount *rootfs_mount;

struct task *new_ts()
{
    for (int i = 0; i < MAX_TASK_NR; ++i) {
        if (task_pool[i].free) {
            task_pool[i].free = 0;
            task_pool[i].pid = i;
            task_pool[i].wd = rootfs_mount->root;
            for (int j = 0; j < 8; ++j)
                task_pool[i].fd_tab[j] = 0;
            return &task_pool[i];
        }
    }
    return 0; // NULL
}

int add_to_ready(struct task *new)
{ // Add new to readyQueue's tail.
    new->status = TASK_RUNNING;

    if (!readyQueue) {
        readyQueue = new;
        new->next = new;
        new->prev = new;
        return 0;
    }
    new->next = readyQueue;
    new->prev = readyQueue->prev;
    readyQueue->prev->next = new;
    readyQueue->prev = new;

    return 0;
}

int rm_from_ready(struct task *old)
{
    if (old->next == old) {
        return -1; // readyQueue can't be empty.
    }
    old->prev->next = old->next;
    old->next->prev = old->prev;
    old->next = 0;
    old->prev = 0;
    return 0;
}

struct mm_struct mm_struct_pool[MAX_TASK_NR];
struct vm_area_struct vma_pool[MAX_VMA_NR];

int init_mms_pool()
{
    for (int i = 0; i < MAX_TASK_NR; ++i)
        mm_struct_pool[i].cpio_start = 0;
    return 0;
}

struct mm_struct *new_mm_struct()
{
    for (int i = 0; i < MAX_TASK_NR; ++i) {
        if (!(mm_struct_pool[i].cpio_start)) {
            mm_struct_pool[i].cpio_start = (char*)1;
            mm_struct_pool[i].page_nr = 0;
            mm_struct_pool[i].mmap = 0;
            return &mm_struct_pool[i];
        }
    }
    return 0; // NULL
}

int init_vma_pool()
{
    for (int i = 0; i < MAX_VMA_NR; ++i)
        vma_pool[i].vm_end = 0;
    return 0;
}


struct vm_area_struct *new_vma()
{
    for (int i = 0; i < MAX_VMA_NR; ++i) {
        if (!(vma_pool[i].vm_end)) {
            vma_pool[i].vm_end = 1;
            vma_pool[i].vm_next = 0;
            return &vma_pool[i];
        }
    }
    return 0; // NULL
}


struct task *thread_create(unsigned long func_addr, unsigned long args)
{
    struct task *new;

    if (!(new = new_ts()))
        return 0; // null
    /* Setting task struct */
    new->kernel_stack_page = kmalloc(0x1000); // 4kb
    new->ksp = new->kernel_stack_page + 0x1000 - sizeof(struct trap_frame); // 16 alignment
    // new->user_stack_page = kmalloc(0x1000); // 4kb
    // new->usp = new->user_stack_page + 0x1000; // 16 alignment
    new->usp = (char*)0x0000fffffffffff0; // 16 alignment
    new->ctx.x19 = func_addr;
    new->ctx.x20 = args;
    new->ctx.x30 = (unsigned long)ret_from_fork; // pc
    new->ctx.sp = (unsigned long)new->ksp; // TODO: user space stack pointer.
    new->status = TASK_NEW;
    new->flag = 0;
    new->priority = 3;
    new->counter = new->priority;
    new->preemptable = 0;
    new->sig.sigpend = 0;
    for (int i = 0; i < MAX_SIG_NR; ++i)
        new->sig.user_handler[i] = 0;
    new->mm = new_mm_struct();
    new->mm->pgd = alloc_page_table(new->mm);

    add_to_ready(new);
    return new;
}
void imhere()
{
    uart_send_string("I'm here\r\n");
}
struct task *pick_next()
{
    struct task *ret;

    ret = readyQueue;
    readyQueue = readyQueue->next;

    return ret;
}

int schedule()
{
    struct task *next, *prev;

    prev = current;
    next = pick_next();
    if (prev == next) // Need no sched.
        return 0;
    current = next;
    set_ttbr0(KVA_TO_PA((unsigned long)(current->mm->pgd)));
    cpu_switch_to(prev, next);
    enable_irq();
    return 0;
}

int sys_exit()
{
    if (rm_from_ready(current) == -1)
        return -1;
    current->status = TASK_ZOMBIE;
    for (int i = 0; i < 8; ++i)
        sys_close(i);
    // Release all resources, except stack and struct task.
    schedule();
    return 0;
}