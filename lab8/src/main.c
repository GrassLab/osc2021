# include "uart.h"
# include "oneshot_timer.h"
# include "buddy.h"
# include "shell.h"
# include "mem.h"
# include "timer.h"
# include "exception.h"
# include "schedule.h"
# include "vfs.h"
# include "my_math.h"
# include "my_string.h"
# include "sdhost.h"
# include "fat32.h"
# include "typedef.h"
//# include "page.h"

extern unsigned char __kernel_start;
extern unsigned char __kernel_end;

int main(){
  uart_init();
  core_timer_init();
  oneshot_timer_init();
  buddy_init();
  buddy_dma_init();
  mem_init();
  vfs_init();
  sd_init();
  sdload();
  task_init();
  
# ifdef __QEMU__
  uart_puts("[Qemu mode]\n");
# else
  uart_puts("[Rpi mode]\n");
# endif
  
  char ct[20];
  uart_puts("[ADDRESSES] :\n");
  uart_puts("\t[Kernel]\n\t  start : ");
  int_to_hex((uint64_t)&__kernel_start, ct);
  uart_puts(ct);
  uart_puts("\n\t  end   : ");
  int_to_hex((uint64_t)&__kernel_end, ct);
  uart_puts(ct);
  uart_puts("\n\t[Buddy]\n\t  start     : ");
  int_to_hex(BUDDY_BASE_ADDR, ct);
  uart_puts(ct);
  uart_puts("\n\t  page size : ");
  int_to_str(BUDDY_PAGE_SIZE, ct);
  uart_puts(ct);
  uart_puts("\n\t  page nums : ");
  int_to_str(BUDDY_PAGE_NUM, ct);
  uart_puts(ct);
  uart_puts("\n");
  
  page_test();
  
  uart_puts(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_puts("Hi!\n");
  uart_puts("Welcome to Eric's system ~\n");
  uart_puts("(Lab8)\n");
  uart_puts(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_flush();
  IRQ_ENABLE();

  schedule();
  return 0;
}
