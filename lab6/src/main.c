# include "uart.h"
# include "oneshot_timer.h"
# include "buddy.h"
# include "shell.h"
# include "mem.h"
# include "timer.h"
# include "exception.h"
# include "schedule.h"


int main(){
  core_timer_init();
  uart_init();
  //uart_puts((char *)"aaa\n");
  oneshot_timer_init();
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
