#include "proc/task.h"
#include "proc.h"
#include "proc/argv.h"
#include "proc/exec.h"
#include "proc/sched.h"

#include "bool.h"
#include "fs/vfs.h"
#include "list.h"
#include "mm.h"
#include "mm/frame.h"
#include "string.h"
#include "syscall.h"
#include "timer.h"
#include "uart.h"

#include "config.h"
#include "log.h"

#include <stdint.h>

#ifdef CFG_LOG_PROC_TASK
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

static void foo();

extern void fork_child_eret();
static inline void close_fd(struct task_struct *task, int fd);

uint32_t new_tid = 0;

struct task_struct *task_create(void *func) {
  struct task_struct *t;
  t = (struct task_struct *)kalloc(sizeof(struct task_struct));
  if (t == NULL) {
    log_println("[task] oops cannot allocate thread");
    return NULL;
  }

  t->kernel_stack = (uintptr_t)kalloc(FRAME_SIZE);

  // Normal task is a kernel function, which has already been loaded to memory
  t->code = NULL;
  t->code_size = 0;
  t->cpu_context.fp = t->kernel_stack + FRAME_SIZE;
  t->cpu_context.lr = (uint64_t)func;
  t->cpu_context.sp = t->kernel_stack + FRAME_SIZE;
  t->status = TASK_STATUS_ALIVE;
  t->id = new_tid++;

  t->fd_size = 0;
  for (int i = 0; i < TASK_MX_NUM_FD; i++) {
    t->fd[i] = NULL;
  }

  // A task would only bind to a user thread if called with exec_user
  t->user_stack = (uintptr_t)(NULL);
  t->user_sp = (uintptr_t)(NULL);

  struct task_entry *entry =
      (struct task_entry *)kalloc(sizeof(struct task_entry));
  entry->task = t;
  list_push(&entry->list, &run_queue);
  // uart_println("entry created: %x %x", entry->task, entry->list);

  log_println("task created: id:%d struct:%x func:%x", t->id, t,
              t->cpu_context.lr);
  return t;
}

void task_free(struct task_struct *task) {
  // TODO: manage parent code free and child code free
  // if (task->code) {
  //   kfree(task->code);
  // }
  if (task->user_stack) {
    kfree((void *)task->user_stack);
  }
  kfree((void *)task->kernel_stack);

  // Close all file descriptors
  for (int i = 0; i < TASK_MX_NUM_FD; i++) {
    if (task->fd[i] != NULL) {
      close_fd(task, i);
    }
  }
  kfree(task);
}

int sys_getpid() {
  struct task_struct *task = get_current();
  return task->id;
};

// Overwrite current user task and kernel task
int sys_exec(const char *name, char *const args[]) {
  exec_user(name, args);
  // a scucess exec_user might never return
  return -1;
}

int sys_open(const char *pathname, int flags) {
  struct task_struct *task = get_current();
  struct file *file;
  if (task->fd_size >= TASK_MX_NUM_FD) {
    return SYS_OPEN_MX_FD_REACHED;
  }
  if (NULL == (file = vfs_open(pathname, flags))) {
    return SYS_OPEN_FILE_NOT_FOUND;
  }
  for (int i = 0; i < TASK_MX_NUM_FD; i++) {
    if (task->fd[i] == NULL) {
      task->fd[i] = file;
      task->fd_size++;
      return i;
    }
  }
  // Should not this line
  return -1;
}

void close_fd(struct task_struct *task, int fd) {
  if (task->fd[fd] != NULL) {
    vfs_close(task->fd[fd]);
    task->fd[fd] = NULL;
  }
  // check for bugs
  task->fd_size--;
  if (task->fd_size < 0) {
    uart_println("[Task free] BUG!!! fd not synce");
    while (1) {
      ;
    }
  }
}

int sys_close(int fd) {
  struct task_struct *task = get_current();
  if ((fd < 0 || fd > TASK_MX_NUM_FD) || (task->fd[fd] == NULL)) {
    return -1;
  }
  close_fd(task, fd);
  return 0;
}

int sys_write(int fd, const void *buf, int count) {
  struct task_struct *task = get_current();
  if ((fd < 0 || fd > TASK_MX_NUM_FD) || (task->fd[fd] == NULL)) {
    return -1;
  }
  struct file *file = task->fd[fd];
  return file->f_ops->write(file, buf, count);
}

int sys_read(int fd, void *buf, int count) {
  struct task_struct *task = get_current();
  if ((fd < 0 || fd > TASK_MX_NUM_FD) || (task->fd[fd] == NULL)) {
    return -1;
  }
  struct file *file = task->fd[fd];
  return file->f_ops->read(file, buf, count);
}

// note: void exit();
void sys_exit() { cur_task_exit(); }

void cur_task_exit() {
  // Mark current thread as exited, which would be picked up by the idle thread
  struct task_struct *task = get_current();
  task->status = TASK_STATUS_DEAD;
  log_println("[task] exit called: %d", task->id);
  task_schedule();
}

void foo() {
  struct task_struct *task = get_current();
  for (int i = 0; i < 2; ++i) {
    uart_println("Thread id: %d -> loop:%d", task->id, i);
    _wait();
    task_schedule();
  }
  cur_task_exit();
}

// This function should be runned as a kernel task
void task_start_user() {
  uart_println("enter user startup");
  char *name = "./argv_test.out";
  char *args[4] = {"./argv_test.out", "-o", "arg2", NULL};

  // Launch a user program thread (in el0) with this kernel thread
  //
  // Eversince this point, exceptions under el0 would be trap
  // to the context of this kernel thread
  // (Since we would call `eret` under this kernel thread)
  exec_user(name, args);
}

void test_user_io() {
  char *name = "./file.out";
  char *args[4] = {"./file.out", NULL};
  exec_user(name, args);
}

void test_tasks() {
  proc_init();

  struct task_struct *root_task;
  root_task = task_create(idle);
  asm volatile("msr tpidr_el1, %0\n" ::"r"((uint64_t)root_task));

  // create a task to bootup the very first user program
  // task_create(task_start_user);
  task_create(test_user_io);

  task_create(foo);
  // task_create(foo);
  // task_create(foo);
#ifdef CFG_LOG_PROC_SCHED
  _dump_runq();
#endif
  idle();
}