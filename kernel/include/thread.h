#ifndef _THREAT_H
#define _THREAT_H

#include "def.h"
typedef enum {
    RUNNING,
    WAITING,
    DEAD
} STATE;

struct context {
    uint64_t reg[29];
    uint64_t fp;
    uint64_t lr;
};

struct Thread {
    // struct context context; // cpu context before switch
    int pid; // process id
    int tid; // thread id
    STATE state;
    uint64_t user_sp;
    uint64_t kernel_sp;

    void * code;
    uint64_t fp;

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

#endif