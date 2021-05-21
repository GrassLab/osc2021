#include "exception.h"

#include <stddef.h>

#include "mini_uart.h"
#include "printf.h"
#include "string.h"
#include "thread.h"
#include "timer.h"

int count = 0;

void enable_interrupt() { asm volatile("msr DAIFClr, 0xf"); }

void disable_interrupt() { asm volatile("msr DAIFSet, 0xf"); }

void sync_handler_currentEL_ELx() {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  printf("SPSR_EL1: 0x%08x\n", spsr_el1);
  printf("ELR_EL1: 0x%08x\n", elr_el1);
  printf("ESR_EL1: 0x%08x\n", esr_el1);
}

void sync_handler_lowerEL_64(uint64_t sp) {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  // printf("sync, SPSR_EL1: 0x%08x\n", spsr_el1);
  // printf("ELR_EL1: 0x%08x\n", elr_el1);
  // printf("ESR_EL1: 0x%08x\n", esr_el1);

  uint32_t ec = (esr_el1 >> 26) & 0x3f;
  // printf("EC: %x\n", ec);
  if (ec == 0b010101) {  // SVC instruction
    uint32_t iss = esr_el1 & ((1 << 25) - 1);
    // printf("syscall number: %d\n", iss);
    trap_frame_t *trap_frame = (trap_frame_t *)sp;

    if (iss == SYS_UART_READ) {
      uint32_t size =
          uart_gets((char *)trap_frame->x[0], (uint32_t)trap_frame->x[1]);
      trap_frame->x[0] = size;
    } else if (iss == SYS_UART_WRITE) {
      uart_puts((char *)trap_frame->x[0]);
      trap_frame->x[0] = trap_frame->x[1];
    } else if (iss == SYS_GETPID) {
      uint32_t pid = get_current()->pid;
      trap_frame->x[0] = pid;
    } else if (iss == SYS_FORK) {
      fork(sp);
    } else if (iss == SYS_EXEC) {
      exec((char *)trap_frame->x[0], (const char **)trap_frame->x[1]);
    } else if (iss == SYS_EXIT) {
      exit();
    } else if (iss == SYS_OPEN) {
      struct file *file =
          vfs_open((char *)trap_frame->x[0], (int)trap_frame->x[1]);
      int fd = thread_get_fd(file);
      trap_frame->x[0] = fd;
    } else if (iss == SYS_CLOSE) {
      struct file *file = thread_get_file((int)trap_frame->x[0]);
      int result = vfs_close(file);
      trap_frame->x[0] = result;
    } else if (iss == SYS_WRITE) {
      struct file *file = thread_get_file((int)trap_frame->x[0]);
      size_t size = vfs_write(file, (const void *)trap_frame->x[1],
                              (size_t)trap_frame->x[2]);
      trap_frame->x[0] = size;
    } else if (iss == SYS_READ) {
      struct file *file = thread_get_file((int)trap_frame->x[0]);
      size_t size =
          vfs_read(file, (void *)trap_frame->x[1], (size_t)trap_frame->x[2]);
      trap_frame->x[0] = size;
    }
  }
}

void irq_handler_currentEL_ELx() {
  disable_interrupt();
  uint32_t is_uart =
      (*CORE0_IRQ_SOURCE & GPU_IRQ) && (*IRQ_PENDING_1 & AUX_IRQ);
  uint32_t is_core_timer = (*CORE0_IRQ_SOURCE & CNTPNS_IRQ);

  if (is_uart) {
    uart_handler();
  } else if (is_core_timer) {
    core_timer_handler_currentEL_ELx();
  }
  enable_interrupt();
}

void irq_handler_lowerEL_64() {
  disable_interrupt();
  uint32_t is_uart =
      (*CORE0_IRQ_SOURCE & GPU_IRQ) && (*IRQ_PENDING_1 & AUX_IRQ);
  uint32_t is_core_timer = (*CORE0_IRQ_SOURCE & CNTPNS_IRQ);

  if (is_uart) {
    uart_handler();
  } else if (is_core_timer) {
    core_timer_handler_lowerEL_64();
  }
  enable_interrupt();
}

void default_handler() { printf("===== default handler =====\n"); }
