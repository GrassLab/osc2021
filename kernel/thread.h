#ifndef THREAD_H
#define THREAD_H

#include "../lib/type.h"

#define THREAD_SIZE 4096

enum Thread_State
{
    Thread_Wait,
    Thread_Idle,
    Thread_Exit
};

struct Context {
    unsigned long regs[10];
    unsigned long fp;
    unsigned long lr;
    unsigned long sp;
};

struct Thread {
    struct Context context;

    int id;
    bool used;

    enum Thread_State state;

    struct Thread * next;
};

struct RunQueue {
    struct Thread * begin;
    struct Thread * end;
};

void thread_init();
void thread_test(int test_id);

// start.S
void switch_to(struct Thread * prev, struct Thread * next);

#endif