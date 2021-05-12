#pragma once

#include "utils.h"

#define THREAD_SIZE 4096

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
} cpu_context;

typedef enum {
  ALIVE,
  ACTIVE,
  DEAD,
  WAIT,
} thread_status;

typedef struct thread_info {
  cpu_context context;
  uint32_t tid;
  thread_status status;
  struct thread_info *next;
} thread_info;

typedef struct {
  thread_info *head, *tail;
} thread_queue;

thread_queue run_queue;
uint32_t thread_cnt;

extern uint64_t get_current();
extern void switch_to(uint64_t, uint64_t);

void foo();
void thread_test();
void thread_init();
thread_info *thread_create(void (*func)());
void run_queue_push(thread_info *thread);
void schedule();
void idle();
void exit();
void kill_zombies();
thread_info *current_thread();
