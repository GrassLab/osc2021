#ifndef SCHED_H
#define SCHED_H

#include "util.h"

#define KERN_STACK_SIZE (PAGE_SIZE * 2)
#define KERN_STACK_SIZE_CTZ (PAGE_SIZE_CTZ + 1)
extern unsigned long preemt_sched;

// c

void init_sched();
void schedule();
unsigned long get_new_pid();
void thread_create(void *func);
void idle();
void die();
void reap_dead();
unsigned long clone();
void sleep(double sec);
void exc_lvl_gain();
void exc_lvl_consume();
void clear_task_timer();

// asm

unsigned long get_pid();
void set_pid(unsigned long);
void *get_sp();
void thread_start();
void switch_exec(void **new_sp, unsigned long new_pid, void **old_sp);
void switch_exec_clone(void **new_sp, unsigned long new_pid, void **old_sp1,
                       void **old_sp2);

#endif