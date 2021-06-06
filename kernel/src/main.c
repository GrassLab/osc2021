#include "alloc.h"
#include "mini_uart.h"
#include "mmu.h"
#include "printf.h"
#include "shell.h"
#include "thread.h"
#include "timer.h"
#include "vfs.h"
int a;
void main() {
  uart_init();
  buddy_init();
  timeout_event_init();
  thread_init();
  vfs_init();
  printf("0x%llx\n", &a);
  int b;
  printf("%p\n", &b);

  uint64_t *pmd = (uint64_t *)PMD_BASE;
  uint64_t *pte1 = (uint64_t *)(PTE_BASE + 503 * 0x1000);
  uint64_t *pte2 = (uint64_t *)(PTE_BASE + 504 * 0x1000);
  printf("0x%llx\n", *((uint64_t *)PGD_BASE));
  printf("0x%llx\n", *(pmd + 0));
  printf("0x%llx\n", *(pmd + 1));
  printf("0x%llx\n", *(pte1 + 510));
  printf("0x%llx\n", *(pte1 + 511));
  printf("0x%llx\n", *(pte2 + 0));
  printf("0x%llx\n", *(pte2 + 1));

  run_shell();
}
