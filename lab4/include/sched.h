#ifndef __SCHED_H_
#define __SCHED_H_

#define KSTACK_SIZE 0x2000
#define USTACK_SIZE 0x2000

typedef unsigned pid_t;

struct task_struct {
    void *stack;
    void *kstack;
    void *user_prog;
    pid_t pid;
};

void schedule_task();
void run_task();

#endif