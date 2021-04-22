#include "include/mm.h"
#include "include/entry.h"
#include "include/csched.h"
#include "include/mini_uart.h"
#include "utils.h"

struct task task_pool[MAX_TASK_NR];
struct task *readyQueue;
struct task *current = 0;

void init_ts_pool()
{
    for (int i = 0; i < MAX_TASK_NR; ++i)
        task_pool[i].free = 1;
}

struct task *new_ts()
{
    for (int i = 0; i < MAX_TASK_NR; ++i) {
        if (task_pool[i].free) {
            task_pool[i].free = 0;
            task_pool[i].pid = i;
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

int rm_from_queue(struct task *old)
{
    if (old->next == old)
        return -1; // readyQueue can't be empty.
    old->prev->next = old->next;
    old->next->prev = old->prev;
    return 0;
}

struct task *thread_create(unsigned long func_addr, unsigned long args)
{
    struct task *new;

    if (!(new = new_ts()))
        return 0; // null
    /* Setting task struct */
    new->kernel_stack_page = kmalloc(0x1000); // 4kb
    new->ksp = new->kernel_stack_page + 0x1000 - sizeof(struct trap_frame); // 16 alignment
    new->user_stack_page = kmalloc(0x1000); // 4kb
    new->usp = new->user_stack_page + 0x1000; // 16 alignment
    new->ctx.x19 = func_addr;
    new->ctx.x20 = args;
    new->ctx.x30 = (unsigned long)ret_from_fork; // pc
    new->ctx.sp = (unsigned long)new->ksp; // TODO: user space stack pointer.
    new->status = TASK_NEW;
    new->flag = 0;
    new->priority = 3;
    new->counter = new->priority;
    new->preemptable = 0;

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
    cpu_switch_to(prev, next);
    enable_irq();
    return 0;
}

int sys_exit()
{
    if (rm_from_queue(current) == -1)
        return -1;
    current->status = TASK_ZOMBIE;
    // Release all resources, except stack and struct task.
    schedule();
    return 0;
}