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
    unsigned long x19;
    unsigned long x20;
    unsigned long x21;
    unsigned long x22;
    unsigned long x23;
    unsigned long x24;
    unsigned long x25;
    unsigned long x26;
    unsigned long x27;
    unsigned long x28;
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

void log_runqueue();

void switch_to();

#endif