#include "thread.h"

#include "alloc.h"
#include "cpio.h"
#include "exception.h"
#include "printf.h"
#include "utils.h"

void foo() {
  for (int i = 0; i < 4; ++i) {
    printf("Thread id: %d, %d\r\n", get_current()->tid, i);
    delay(1000000);
    schedule();
  }
  exit();
  return;
}

void thread_test1() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  for (int i = 0; i < 5; ++i) {
    thread_create(foo);
  }
  idle();
}

void user_test() {
  const char *argv[] = {"argv_test", "-o", "arg2", 0};
  exec("argv_test", argv);
}

void thread_test2() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  thread_create(user_test);
  idle();
}

void thread_init() {
  run_queue.head = 0;
  run_queue.tail = 0;
  thread_cnt = 0;
}

thread_info *thread_create(void (*func)()) {
  thread_info *thread = (thread_info *)malloc(sizeof(thread_info));
  void *kernel_stack_base = malloc(STACK_SIZE);
  thread->context.fp = (uint64_t)kernel_stack_base + STACK_SIZE;
  thread->context.lr = (uint64_t)func;
  thread->context.sp = (uint64_t)kernel_stack_base + STACK_SIZE;
  thread->user_sp = 0;
  thread->tid = thread_cnt++;
  thread->status = ALIVE;
  thread->next = 0;
  thread->user_stack_base = 0;
  thread->user_program_base = USER_PROGRAM_BASE + thread->tid * USER_PROGRAM_SIZE;
  run_queue_push(thread);
  return thread;
}

void schedule() {
  if (run_queue.head == 0) {
    return;
  }
  if (run_queue.head == run_queue.tail) {  // idle thread
    free(run_queue.head);
    run_queue.head = run_queue.tail = 0;
    thread_cnt = 0;
    enable_interrupt();  // need uart interrupt when go back to shell
    return;
  }

  do {
    run_queue.tail->next = run_queue.head;
    run_queue.tail = run_queue.head;
    run_queue.head = run_queue.head->next;
    run_queue.tail->next = 0;
  } while (run_queue.head->status != ALIVE);
  switch_to(get_current(), run_queue.head);
}

void idle() {
  while (1) {
    kill_zombies();
    schedule();
    if (run_queue.head == 0) break;
  }
  printf("finish\n");
}

void exit() {
  thread_info *cur = get_current();
  cur->status = DEAD;
  schedule();
}

void run_queue_push(thread_info *thread) {
  if (run_queue.head == 0) {
    run_queue.head = run_queue.tail = thread;
  } else {
    run_queue.tail->next = thread;
    run_queue.tail = thread;
  }
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

void exec(const char *program_name, const char **argv) {
  thread_info *cur = get_current();
  if (cur->user_stack_base == 0) {
    cur->user_stack_base = (uint64_t)malloc(STACK_SIZE);
  }
  cur->user_sp = cur->user_stack_base + STACK_SIZE;
  // printf("0x%0x\n", cur->user_program_base);
  cur->user_program_size =
      cpio_load_user_program(program_name, cur->user_program_base);
  // printf("0x%08x  %d\n", cur->user_program_base, cur->user_program_size);
  // printf("copy done\n");

  // pass arguments: argc, **argv, *argv[0], *argv[1], ..., NULL, ...
  int argc = 0, byte_cnt = 0;
  for (int i = 0;; ++i) {
    if (argv[i] == 0) break;
    ++argc;
    for (int j = 0;; ++j) {
      ++byte_cnt;
      if (argv[i][j] == 0) break;
    }
  }
  // printf("argc: %d %d\n", argc, byte_cnt);

  uint32_t arg_size = (2 + argc + 1) * 8 + byte_cnt;
  cur->user_sp -= arg_size;
  cur->user_sp -= (cur->user_sp & 0xf);  // alignment

  char *data = (char *)cur->user_sp;
  *(uint64_t *)data = argc;  // argc
  data += 8;
  *(uint64_t *)data = (uint64_t)(data + 8);  // **argv
  data += 8;
  char *argv_buf = data + 8 * (argc + 1);
  for (int i = 0; i < argc; ++i) {  // *argv[i] & content
    *(uint64_t *)data = (uint64_t)argv_buf;
    for (int j = 0;; ++j) {
      *argv_buf = argv[i][j];
      ++argv_buf;
      if (argv[i][j] == 0) break;
    }
    data += 8;
  }
  *(uint64_t *)data = 0;  // NULL after *argv[i]

  // return to user program
  uint64_t spsr_el1 = 0x0;  // EL0t with interrupt enabled
  uint64_t target_addr = cur->user_program_base;
  uint64_t target_sp = cur->user_sp;
  asm volatile("msr spsr_el1, %0" : : "r"(spsr_el1));
  asm volatile("msr elr_el1, %0" : : "r"(target_addr));
  asm volatile("msr sp_el0, %0" : : "r"(target_sp));
  asm volatile("mrs x3, sp_el0");
  asm volatile("ldr x0, [x3, 0]");
  asm volatile("ldr x1, [x3, 8]");
  asm volatile("eret");
}
