#ifndef __THREAD_H
#define __THREAD_H

#include <stdint.h>
#include "allocator.h"
#include "printf.h"

typedef struct {
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t fp;
    uint64_t lr;
    uint64_t sp;
} Thread_reg;

typedef struct __Thread {
    struct __Thread *next;
    uint64_t pid;
    uint64_t ppid;
    void (*entry)();
    void *kernel_stack;  //fix 1 page
    size_t user_stack_npage;
    void *user_stack;
    uint32_t state;  //0 == exit, 1 == init, 2 == running/idle
    Thread_reg reg;
    uint64_t thread_time;
} Thread;

void idle();
int64_t thread_create(void (*func)());
void exit();
Thread *current_thread();
Thread *get_thread(uint64_t pid);
void kill_zombies();
void schedule();
int64_t fork();

#endif
