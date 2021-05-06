# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "shell.h"
# include "mem.h"
# include "bitset.h"
# include "timer.h"
# include "svc_call.h"
# include "exception.h"
# include "schedule.h"
# include "demo.h"


int main(){
  core_timer_init();
  uart_init();
  //uart_puts((char *)"aaa\n");
  svc_init();
  uart_puts((char *) "Buddy system init\n");
  buddy_init();
  buddy_dma_init();
  mem_init();
  uart_puts((char *) "Hi!\n");
  uart_puts((char *) "Welcome to Eric's system ~\n");
  uart_puts((char *) "(Lab4)\n");
  uart_flush();
  task_init();
  IRQ_ENABLE();

  //task_create(task_demo_1, 4);
  schedule();
  return 0;
}
