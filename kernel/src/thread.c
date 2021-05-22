#include "thread.h"

#include "alloc.h"
#include "cpio.h"
#include "printf.h"
#include "vfs.h"

void foo() {
  for (int i = 0; i < 4; ++i) {
    printf("Thread id: %d, %d\r\n", get_current()->pid, i);
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

void thread_vfs_test() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  thread_create(vfs_test);
  idle();
}

void thread_vfs_ls_test() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  thread_create(vfs_ls_test);
  idle();
}

void thread_vfs_hard_test() {
  thread_info *idle_t = thread_create(0);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)idle_t));
  thread_create(vfs_hard_test);
  idle();
}

void thread_init() {
  run_queue.head = 0;
  run_queue.tail = 0;
  thread_cnt = 0;
}

thread_info *thread_create(void (*func)()) {
  thread_info *thread = (thread_info *)malloc(sizeof(thread_info));
  thread->pid = thread_cnt++;
  thread->status = THREAD_READY;
  thread->next = 0;
  thread->kernel_stack_base = (uint64_t)malloc(STACK_SIZE);
  thread->user_stack_base = 0;
  thread->user_program_base =
      USER_PROGRAM_BASE + thread->pid * USER_PROGRAM_SIZE;
  thread->context.fp = thread->kernel_stack_base + STACK_SIZE;
  thread->context.lr = (uint64_t)func;
  thread->context.sp = thread->kernel_stack_base + STACK_SIZE;
  for (int i = 0; i < FD_MAX; ++i) thread->fd_table.files[i] = 0;
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
    return;
  }

  do {
    run_queue.tail->next = run_queue.head;
    run_queue.tail = run_queue.head;
    run_queue.head = run_queue.head->next;
    run_queue.tail->next = 0;
  } while (run_queue.head->status != THREAD_READY);
  switch_to(get_current(), run_queue.head);
}

void idle() {
  while (1) {
    kill_zombies();
    handle_fork();
    schedule();
    if (run_queue.head == 0) break;
  }
}

void exit() {
  thread_info *cur = get_current();
  cur->status = THREAD_DEAD;
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
    for (thread_info *cur = ptr->next;
         cur != 0 && (cur->status == THREAD_DEAD);) {
      thread_info *tmp = cur->next;
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
  uint64_t user_sp = cur->user_stack_base + STACK_SIZE;
  cur->user_program_size =
      cpio_load_user_program(program_name, cur->user_program_base);

  // parse arguments: argc, **argv, *argv[0], *argv[1], ..., NULL, ...
  int argc = 0, byte_cnt = 0;
  for (int i = 0;; ++i) {
    if (argv[i] == 0) break;
    ++argc;
    for (int j = 0;; ++j) {
      ++byte_cnt;
      if (argv[i][j] == 0) break;
    }
  }
  // put arguments to user stack
  uint32_t arg_size = (2 + argc + 1) * 8 + byte_cnt;
  user_sp -= arg_size;
  user_sp -= (user_sp & 0xf);  // alignment
  char *data = (char *)user_sp;
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
  uint64_t target_sp = user_sp;
  asm volatile("msr spsr_el1, %0" : : "r"(spsr_el1));
  asm volatile("msr elr_el1, %0" : : "r"(target_addr));
  asm volatile("msr sp_el0, %0" : : "r"(target_sp));
  asm volatile("mrs x3, sp_el0");
  // put argc to x0, **argv to x1
  asm volatile("ldr x0, [x3, 0]");
  asm volatile("ldr x1, [x3, 8]");
  asm volatile("eret");
}

void fork(uint64_t sp) {
  run_queue.head->status = THREAD_FORK;
  run_queue.head->trap_frame_addr = sp;
  schedule();
  trap_frame_t *trap_frame = (trap_frame_t *)(get_current()->trap_frame_addr);
  trap_frame->x[0] = run_queue.head->child_pid;
}

void handle_fork() {
  for (thread_info *ptr = run_queue.head->next; ptr != 0; ptr = ptr->next) {
    if ((ptr->status) == THREAD_FORK) {
      thread_info *child = thread_create(0);
      create_child(ptr, child);
      ptr->status = THREAD_READY;
      child->status = THREAD_READY;
    }
  }
}

void create_child(thread_info *parent, thread_info *child) {
  child->user_stack_base = (uint64_t)malloc(STACK_SIZE);
  child->user_program_size = parent->user_program_size;
  parent->child_pid = child->pid;
  child->child_pid = 0;

  char *src, *dst;
  // copy saved context in thread info
  src = (char *)&(parent->context);
  dst = (char *)&(child->context);
  for (uint32_t i = 0; i < sizeof(cpu_context); ++i, ++src, ++dst) {
    *dst = *src;
  }
  // copy kernel stack
  src = (char *)(parent->kernel_stack_base);
  dst = (char *)(child->kernel_stack_base);
  for (uint32_t i = 0; i < STACK_SIZE; ++i, ++src, ++dst) {
    *dst = *src;
  }
  // copy user stack
  src = (char *)(parent->user_stack_base);
  dst = (char *)(child->user_stack_base);
  for (uint32_t i = 0; i < STACK_SIZE; ++i, ++src, ++dst) {
    *dst = *src;
  }
  // copy user program
  src = (char *)(parent->user_program_base);
  dst = (char *)(child->user_program_base);
  for (uint32_t i = 0; i < parent->user_program_size; ++i, ++src, ++dst) {
    *dst = *src;
  }

  // set correct address for child
  uint64_t kernel_stack_base_dist =
      child->kernel_stack_base - parent->kernel_stack_base;
  uint64_t user_stack_base_dist =
      child->user_stack_base - parent->user_stack_base;
  uint64_t user_program_base_dist =
      child->user_program_base - parent->user_program_base;
  child->context.fp += kernel_stack_base_dist;
  child->context.sp += kernel_stack_base_dist;
  child->trap_frame_addr = parent->trap_frame_addr + kernel_stack_base_dist;
  trap_frame_t *trap_frame = (trap_frame_t *)(child->trap_frame_addr);
  trap_frame->x[29] += user_stack_base_dist;    // fp (x29)
  trap_frame->x[30] += user_program_base_dist;  // lr (x30)
  trap_frame->x[32] += user_program_base_dist;  // elr_el1
  trap_frame->x[33] += user_stack_base_dist;    // sp_el0
}

struct file *thread_get_file(int fd) {
  thread_info *cur = get_current();
  return cur->fd_table.files[fd];
}

int thread_get_fd(struct file *file) {
  if (file == 0) return -1;
  thread_info *cur = get_current();
  // find next available fd
  for (int fd = 3; fd < FD_MAX; ++fd) {
    if (cur->fd_table.files[fd] == 0) {
      cur->fd_table.files[fd] = file;
      return fd;
    }
  }
  return -1;
}
