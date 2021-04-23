#pragma once
#include "utils.h"
#include "system.h"
#include "shell.h"

enum thread_status
{
    ACTIVE,
    DEAD,
    ALIVE,
    WAIT,
};
typedef struct thread_info
{
    struct thread_info *prev, *next;
    void *reg[10], *fp, *lr, *sp;
    uint64_t pid, tid;
    enum thread_status status;
} thread_info;

extern void delay(int);
extern uint64_t get_current();
extern void switch_to(void *, void *);

void Thread(void *);
void add_to_run_queue(thread_info *);
void move_to_wait_queue();
thread_info *current_thread();
void schedule();
void exit();
void foo();
void idle();
void kill_zombies();
int available_thread_id();
void init_thread();