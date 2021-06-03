#include "include/scheduler.h"
#include "include/allocator.h"
#include "include/interrupt.h"
#include "include/util.h"
#include "include/uart.h"

static task_struct *run_queue = NULL;
static task_struct *run_queue_tail = NULL;
static task_struct *terminal_queue = NULL;
static task_struct *terminal_queue_tail = NULL;

void push_queue(task_struct *task, task_struct **queue, task_struct **queue_tail, int state) {
    task->state = state;
    if (*queue) {
        task->prev = *queue_tail;
        task->next = NULL;
        (*queue_tail)->next = task;
        (*queue_tail) = task;
    } else {
        (*queue) = task;
        task->next = NULL;
        task->prev = NULL;
        (*queue_tail) = task;
    }
}

void push_run_queue(task_struct *task) {
    push_queue(task, &run_queue, &run_queue_tail, RUN_STATE);
}

void push_terminal_queue(task_struct *task) {
    push_queue(task, &terminal_queue, &terminal_queue_tail, ZOMBIE_STATE);
}

task_struct *pop_queue_head(task_struct **queue, task_struct **queue_tail) {
    task_struct *task;
    task = *queue;
    if (*queue) {
        *queue = (*queue)->next;
        if (*queue)
            (*queue)->prev = NULL;
        else
            *queue_tail = NULL;
    }
    return task;
}

task_struct *pop_terminal_queue_head() {
    return pop_queue_head(&terminal_queue, &terminal_queue_tail);
}

// 0 is success, 1 is fail
int pop_queue_by_pid(task_struct **queue, task_struct **queue_tail, int pid) {
    task_struct *task = *queue;
    while (task) {
        if (task->pid == pid) {
            if (task == *queue)
                *queue = task->next;
            if (task == *queue_tail)
                *queue_tail = task->prev;
            if (task->next)
                task->next->prev = task->prev;
            if (task->prev)
                task->prev->next = task->next;
            return 0;
        }
        task = task->next;
    }
    uart_put_str("Do not find pid: ");
    uart_put_int(pid);
    uart_put_str(" in queue.\n");
    return 1;
}

void pop_run_queue_by_pid(int pid) {
    int ret = pop_queue_by_pid(&run_queue, &run_queue_tail, pid);
    if (ret) {
        uart_put_str("Do not find pid: ");
        uart_put_int(pid);
        uart_put_str(" in run queue.\n");
    }
}

void update_use_time() {
    unsigned long now = get_time();
    current->use_time += (now - current->last_time);
    current->last_time = now;
}

// 1 is no time, 0 is have time
int check_lifetime() {
    update_use_time();
    if (current->use_time >= TASK_LIFE_TIME)
        return 1;
    else
        return 0;
}

task_struct *peek_run_queue() {
    task_struct *task;
    if (check_lifetime()) {
        task = pop_queue_head(&run_queue, &run_queue_tail);
        push_run_queue(task);
        task->use_time = 0;
    }
    task = run_queue;
    return task;
}

void schedule() {
    task_struct *next;
    next = peek_run_queue();
    if (next == current)
        return;
    
    next->use_time = 0;
    next->last_time = get_time();
    switch_to(current, next);
    current->last_time = get_time();
}