#ifndef _THREAT_H
#define _THREAT_H

#define THREAD_STACK_SIZE (5) // 2 ^ exp

#include "def.h"
typedef enum {
    RUNNING,
    WAITING,
    DEAD
} STATE;

struct context {
    uint64_t reg[31];
    uint64_t fp;
    uint64_t lr;
};

struct Thread {
    uint64_t kernel_sp; // point to context
    uint64_t user_sp;


    int pid; // process id
    int tid; // thread id
    STATE state;

    void * code;

    struct Thread *prev;
    struct Thread *next;
};

struct Thread_pool {
    struct Thread *head;
    struct Thread *tail;
    int num_threads;
};

void init_thread_pool();
int get_new_tid();
void thread_pool_add(struct Thread *t);
void create_thread(void(*thread_func)());
void set_current_thread(void *t);
struct Thread * get_current_thread();

#endif