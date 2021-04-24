#pragma once
#include "utils.h"
#include "system.h"
#include "printf.h"

typedef struct thread_info
{
    unsigned long context[13]; // context
    uint64_t tid;
    struct thread_info *next;
    int status;
    unsigned long child_pid;
} thread_info;
#define THREAD_SIZE 4096
#define THREAD_DEAD 1
#define THREAD_FORK 2

extern void delay(int);
extern uint64_t get_current();
extern void switch_to(uint64_t, uint64_t);

thread_info *Thread(void *);
void add_to_run_queue(thread_info *);
thread_info *current_thread();
void schedule();
int fork();
void do_fork();
void copy_program(thread_info *, thread_info *);
void exit();
void foo();
void idle();
void kill_zombies();
void init_thread();
void thread_test();