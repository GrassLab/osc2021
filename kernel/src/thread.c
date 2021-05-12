#include "thread.h"

#include "alloc.h"
#include "printf.h"
#include "utils.h"

void foo() {
  for (int i = 0; i < 4; ++i) {
    printf("Thread id: %d, %d\r\n", current_thread()->tid, i);
    delay(1000000);
    schedule();
  }
  exit();
  return;
}

void thread_test() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  for (int i = 0; i < 5; ++i) {
    thread_create(foo);
  }
  idle();
}

void thread_init() {
  run_queue.head = 0;
  run_queue.tail = 0;
  thread_cnt = 0;
}

thread_info *thread_create(void (*func)()) {
  thread_info *thread = (thread_info *)malloc(THREAD_SIZE);
  thread->context.fp = (uint64_t)thread + THREAD_SIZE;
  thread->context.lr = (uint64_t)func;
  thread->context.sp = (uint64_t)thread + THREAD_SIZE;
  thread->tid = thread_cnt++;
  thread->status = ALIVE;
  thread->next = 0;
  run_queue_push(thread);
  return thread;
}

void run_queue_push(thread_info *thread) {
  if (run_queue.head == 0) {
    run_queue.head = run_queue.tail = thread;
  } else {
    // printf("%p\n", run_queue.tail);
    // printf("%p\n", run_queue.tail->next);
    run_queue.tail->next = thread;
    run_queue.tail = thread;
  }
}

void schedule() {
  if (run_queue.head == 0) {
    return;
  }
  if (run_queue.head == run_queue.tail) {  // idle thread
    // uart_puts("??\r\n");
    free(run_queue.head);
    run_queue.head = run_queue.tail = 0;
    thread_cnt = 0;
    return;
  }

  do {
    run_queue.tail->next = run_queue.head;
    run_queue.tail = run_queue.head;
    run_queue.head = run_queue.head->next;
    run_queue.tail->next = 0;
  } while (run_queue.head->status != ALIVE);
  // unsigned long sp_addr;
  // asm volatile("ldr %0, [sp]\n":"=r"(sp_addr):);
  // printf("[schedule]svc, sp: %x\n", sp_addr);
  switch_to(get_current(), (uint64_t)&run_queue.head->context);
}

void idle() {
  while (1) {
    kill_zombies();
    schedule();
  }
}

void exit() {
  thread_info *cur = current_thread();
  cur->status = DEAD;
  schedule();
}

void kill_zombies() {
  if (run_queue.head == 0) return;
  for (thread_info *ptr = run_queue.head; ptr->next != 0; ptr = ptr->next) {
    for (thread_info *cur = ptr->next; cur != 0 && cur->status == DEAD;) {
      thread_info *tmp = cur->next;
      // printf("find dead thread %d\n", cur->tid);
      free((void *)cur);
      ptr->next = tmp;
      cur = tmp;
    }
    if (ptr->next == 0) {
      run_queue.tail = ptr;
      break;
    }
  }
}

thread_info *current_thread() {
  thread_info *ptr;
  asm volatile("mrs %0, tpidr_el1\n" : "=r"(ptr) :);
  return ptr;
}
