#include "proc.h"
#include "proc/task.h"

#include "bool.h"
#include "fs/vfs.h"
#include "mm.h"
#include "mm/frame.h"
#include "string.h"
#include "syscall.h"

#include "config.h"
#include "log.h"

#include <stdint.h>

#ifdef CFG_LOG_PROC_TASK
static const int _DO_LOG = 1;
#else
static const int _DO_LOG = 0;
#endif

extern void fork_child_eret();

int sys_fork(const struct trap_frame *tf) {

  struct task_struct *parent = get_current();

  {
    // just logging
    uintptr_t el1_sp;
    asm volatile("mov %0, sp" : "=r"(el1_sp));
    log_println("current sp(sp_el1): %x", el1_sp);
  }

  struct task_struct *child = task_create(NULL);

  // USER STACK
  child->user_stack = (uintptr_t)kalloc(FRAME_SIZE);
  memcpy((char *)child->user_stack, (const char *)parent->user_stack,
         FRAME_SIZE);

  // KERNEL STACK
  memcpy((char *)child->kernel_stack, (const char *)parent->kernel_stack,
         FRAME_SIZE);

  // CODE
  // TODO: fix this (memory leak)
  // parent should free memory
  // parent should be collected after child
  child->code = NULL;

  // Child return
  // direct to the exception return point
  uintptr_t kstack_tf_offset = ((uintptr_t)tf) - parent->kernel_stack;
  struct trap_frame *child_tf =
      (struct trap_frame *)(child->kernel_stack + kstack_tf_offset);
  child->cpu_context.sp = (uintptr_t)child_tf;
  child->cpu_context.lr = (uint64_t)fork_child_eret;
  child_tf->regs[0] = 0;

  log_println("parent kstack:%x ustack:%x", parent->kernel_stack,
              parent->user_stack);
  log_println("   tf:%x ctx.fp:%x ctx.sp:%x ctx.lr: %x", tf,
              parent->cpu_context.fp, parent->cpu_context.sp,
              parent->cpu_context.lr);

  log_println("child kstack:%x ustack:%x ", child->kernel_stack,
              child->user_stack);
  log_println("   tf:%x ctx.fp:%x ctx.sp:%x ctx.lr: %x", child_tf,
              child->cpu_context.fp, child->cpu_context.sp,
              child->cpu_context.lr);

  return child->id;
}
