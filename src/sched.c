#include "sched.h"
#include "io.h"
#include "mm.h"
#include "entry.h"
#include "string.h"
#include "utility.h"
#include "time.h"

#define MAX_TASK_NUM 50

struct task_struct *current_task;

struct task_queue waiting_queue = {
    .head = NULL,
    .tail = NULL,
    .num = 0,
};

struct task_queue unready_queue = {
    .head = NULL,
    .tail = NULL,
    .num = 0,
};

struct task_queue running_queue = { .head = NULL,
    .tail = NULL,
    .num = 0,
};

struct task_struct task_slot[MAX_TASK_NUM];

struct task_struct *task_queue_pop_head (struct task_queue *q) {
    struct task_struct *ptr = q->head;
    if (!ptr)
        return NULL;

    q->num--;
    q->head = ptr->next;
    if (!q->head)
        q->tail = NULL;
    ptr->next = NULL;
    return ptr;
}

void task_queue_add_task (struct task_queue *q, struct task_struct *t) {

    if (q->head == NULL)
        q->head = t;

    if (q->tail)
        q->tail->next = t;

    t->next = NULL;
    q->tail = t;
    q->num++;
}

void init_sched () {
    current_task = NULL;

    for (int i = 0; i < MAX_TASK_NUM; i++) {
        task_slot[i].id = i;
        task_queue_add_task(&unready_queue, &(task_slot[i]));
    }
}

void fork_thread (struct trap_frame *tf) {
    struct task_struct *t = task_queue_pop_head(&unready_queue);
    t->timestamp = 0;
    if (!t) {
        tf->x0 = ~(u64)0;
    }

    t->stack_top = bs_malloc(STACK_SIZE);
    t->kstack_top = bs_malloc(STACK_SIZE);
    t->kcontext.sp = (u64)(t->kstack_top + STACK_SIZE - context_switch_size);
    memcpy((u8 *)(t->kcontext.sp), (u8 *)tf, context_switch_size);
    /* copy stack data */
    memcpy((u8 *)(t->stack_top), (u8 *)(current_task->stack_top), STACK_SIZE);

    struct trap_frame *tmp = (struct trap_frame *)(t->kcontext.sp);
    tf->x0 = t->id; // parent process will get child process's id
    tmp->x0 = 0;
    // TODO: modify child's sp and lr registers

    t->kcontext.lr = get_new_task_entry();
    task_queue_add_task(&running_queue, t);
}

void create_thread (struct trap_frame *tf, void *addr) {
    struct task_struct *t = task_queue_pop_head(&unready_queue);
    t->timestamp = 0;
    if (!t) {
        tf->x0 = ~(u64)0;
    }
    t->stack_top = bs_malloc(STACK_SIZE);
    t->kstack_top = bs_malloc(STACK_SIZE);
    t->kcontext.sp = (u64)(t->kstack_top + STACK_SIZE - context_switch_size);
    t->kcontext.lr = get_new_task_entry();

    struct trap_frame *tmp = (struct trap_frame *)(t->kcontext.sp);
    tf->x0 = t->id;
    tmp->x0 = 0;
    tmp->sp = (u64)(t->stack_top + STACK_SIZE);
    tmp->lr = (u64)addr;

    task_queue_add_task(&running_queue, t);
}


int sched_add_running_task () {
    if (unready_queue.num) {
        unready_queue.num--;
        struct task_struct *ptr = unready_queue.head;
        unready_queue.head = ptr->next;
        if (!unready_queue.head)
            unready_queue.tail = NULL;
    }
    return -1;
}

void activate_waiting_queue () {
    float current_time = get_time();
    struct task_queue tmp = waiting_queue;
    waiting_queue.head = NULL;
    waiting_queue.tail = NULL;
    waiting_queue.num = 0;

    while (1) {
        struct task_struct *t = task_queue_pop_head(&tmp);
        if (!t)
            break;
        if (t->timestamp > current_time)
            task_queue_add_task(&waiting_queue, t);
        else
            task_queue_add_task(&running_queue, t);
    }
}

enum SCHEDULE_FLAG {
    SLEEP,
    RUN,
    KILL
};

void _schedule () {
    activate_waiting_queue();
    struct task_struct *candidate = task_queue_pop_head(&running_queue);
    struct task_struct *current = current_task;
    /* no switchable threads */
    if (!candidate)
        return;

    /* switch tasks */
    current_task = candidate;

    if (current->flag == RUN)
        task_queue_add_task(&running_queue, current);
    else if (current->flag == SLEEP)
        task_queue_add_task(&waiting_queue, current);

    /* context switch */
    switch_to(&(current->kcontext), &(candidate->kcontext));
}


void schedule () {
    current_task->flag = RUN;
    _schedule();
}

/* wait nano seconds */
void schedule_wait (unsigned long time) {
    current_task->timestamp = get_time() + (double)time / 1000000000;
    current_task->flag = SLEEP;
    _schedule();
}

void _get_pid (struct trap_frame * tf) {
    tf->x0 = current_task->id;
}
