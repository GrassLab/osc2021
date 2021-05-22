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


int main(){
  core_timer_init();
  uart_init();
  //uart_puts((char *)"aaa\n");
  oneshot_timer_init();
  buddy_init();
  buddy_dma_init();
  mem_init();
  vfs_init();
  task_init();
  
# ifdef __QEMU__
  uart_puts((char *) "[Qemu mode]\n");
# else
  uart_puts((char *) "[Rpi mode]\n");
# endif

  uart_puts((char *) ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_puts((char *) "Hi!\n");
  uart_puts((char *) "Welcome to Eric's system ~\n");
  uart_puts((char *) "(Lab6)\n");
  uart_puts((char *) ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_flush();
  
  IRQ_ENABLE();

  //task_create(task_demo_1, 4);
  schedule();
  return 0;
}
