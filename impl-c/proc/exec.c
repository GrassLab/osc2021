#include "proc/exec.h"
#include "proc/argv.h"
#include "proc/task.h"

#include "cpio.h"
#include "mm.h"
#include "mm/frame.h"
#include "stddef.h"
#include "stdint.h"
#include "timer.h"
#include "uart.h"

#include "cfg.h"
#include "log.h"

#ifdef CFG_LOG_PROC_EXEC
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

// load program into a seperate memory space
void *load_program(const char *name, /*ret*/ size_t *target_size) {
  unsigned long size;
  uint8_t *file = (uint8_t *)cpioGetFile((void *)RAMFS_ADDR, name, &size);
  if (file == NULL) {
    uart_println("[Loader]Cannot found `%s` under rootfs", name);
    return NULL;
  }
  if (target_size != NULL) {
    *target_size = size;
  }
  unsigned char *load_addr = (unsigned char *)kalloc(FRAME_SIZE);
  for (unsigned long i = 0; i < size; i++) {
    load_addr[i] = file[i];
  }
  return load_addr;
}

void exec_user(const char *name, char **argv) {
  struct task_struct *task = get_current();
  log_println("[exec] name:%s cur_task: %d(%x)", name, task->id, task);

  // unload previous task code
  if (task->code) {
    log_println("[exec] free code from previous process: %x", task->code);
    kfree(task->code);
    task->code_size = 0;
  }

  // address of the program code in memory
  void *entry_point = load_program(name, &task->code_size);
  log_println("[exec] load new program code at: %x", task->code);
  task->code = entry_point;

  // context under kernel mode
  task->cpu_context.fp = (uint64_t)task + FRAME_SIZE;
  task->cpu_context.lr = (uint64_t)entry_point;
  task->cpu_context.sp = (uint64_t)task + FRAME_SIZE;

  // call stack under user mode
  task->user_stack = (uintptr_t)kalloc(FRAME_SIZE);
  task->user_sp = task->user_stack + FRAME_SIZE;

  // place args
  int argc;
  char **user_argv;
  uintptr_t new_sp;
  place_args(task->user_sp, argv, &argc, &user_argv, &new_sp);
  task->user_sp = new_sp;

  // Jump into user mode
  asm volatile("mov x0, 0x340  \n"); // enable core timer interrupt
  asm volatile("msr spsr_el1, x0  \n");
  asm volatile("msr sp_el0, %0    \n" ::"r"(task->user_sp));
  asm volatile("msr elr_el1, %0   \n" ::"r"(task->cpu_context.lr));

  asm volatile("mov x0, %0 \n\
                mov x1, %1 \n\
                eret" ::"r"(argc),
               "r"(user_argv));
}