#ifndef SCHED_H
#define SCHED_H

#include "mem.h"
#include "util.h"

#define KERN_STACK_SIZE (PAGE_SIZE * 2)
#define KERN_STACK_SIZE_CTZ (PAGE_SIZE_CTZ + 1)

void init_sched();
void schedule();
unsigned long get_new_pid();

void thread_create(void *func);
void idle();
void die();

unsigned long get_pid();
void set_pid(unsigned long);
void *get_sp();
void switch_exec(void *new_sp, unsigned long new_pid, void **old_sp);
void reap_dead(cdl_list *reap_list);
unsigned long clone();
void thread_start();

void switch_exec_clone(void *new_sp, unsigned long new_pid,
                       void **old_sp1_store, void *old_sp2_sp,
                       void **old_sp2_store);

void sleep(double sec);

extern unsigned long preemt_sched;

void exc_lvl_inc();
void exc_lvl_consume();

void clear_task_timer();

#endif