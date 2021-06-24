#ifndef THREAD_H
#define THREAD_H

#include "fd.h"
#include "../../lib/type.h"

#define THREAD_SIZE 4096

enum Thread_State
{
    Thread_Wait,
    Thread_Idle,
    Thread_Exit
};

struct Trap_Frame {
    unsigned long regs[32];
    unsigned long sp_el0;
    unsigned long elr_el1;
    unsigned long spsr_el1;
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

    unsigned long program_addr;
    enum Thread_State state;

    struct fd_node fd_table[FD_TABLE_SIZE];

    struct Thread * next;
};

struct RunQueue {
    struct Thread * begin;
    struct Thread * end;
};

void thread_init();
void thread_exit();
void thread_fork(struct Trap_Frame * tf);

struct Thread * current_thread();

void thread_test(int test_id);

void log_runqueue();

void switch_to();

void child_return_from_fork(); // start.S

#endif