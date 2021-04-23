#pragma once
#include "list.h"

struct cpu_context {
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
  unsigned long fp; // x29
  unsigned long lr; // x30
  unsigned long sp; // stack pointer
};

/**
 * task_struct
 * Each task actually occupy a entire memory frame
 * | task_struct|
 * |     --     |
 * |     ...    |
 * |    stack   |
 * |     ...    |
 */
struct task_struct {
  struct cpu_context cpu_context;
  unsigned long id;
  int status;
  //
  // stack
  //
};

#define TASK_STATUS_DEAD 0
#define TASK_STATUS_ALIVE 1

/* sched.c */
void test_tasks();
void scheduler_init();

/* sched.S */
//
void switch_to(struct task_struct *prev, struct task_struct *next);

static inline struct task_struct *get_current() {
  unsigned long cur;
  asm volatile("mrs %0, tpidr_el1 \n" : "=r"(cur) :);
  return (struct task_struct *)cur;
}

static inline void _wait() {
  for (uint64_t j = 0; j < (1 << 27); j++) {
    ;
  }
}