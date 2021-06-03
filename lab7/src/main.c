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


int main(){
  core_timer_init();
  uart_init();
  //uart_puts((char *)"aaa\n");
  oneshot_timer_init();
  buddy_init();
  buddy_dma_init();
  mem_init();
  vfs_init();
  sd_init();
  sdload();
  task_init();

  
# ifdef __QEMU__
  uart_puts((char *) "[Qemu mode]\n");
# else
  uart_puts((char *) "[Rpi mode]\n");
# endif
  /*
  char name_test[10] = "新";
  //name_test[1] = 0x65;
  //name_test[2] = 0xb0;
  for (int i=0; i<str_len(name_test); i++){
    char ct[10];
    int_to_hex(name_test[i], ct);
    uart_puts(ct);
    uart_puts("\n");
  }
  uart_puts((char *) "\u65b0\n");
  uart_puts(name_test);
  uart_puts("\n");
  */
  uart_puts((char *) ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_puts((char *) "Hi!\n");
  uart_puts((char *) "Welcome to Eric's system ~\n");
  uart_puts((char *) "(Lab7)\n");
  uart_puts((char *) ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
  uart_flush();
  IRQ_ENABLE();

  //task_create(task_demo_1, 4);
  schedule();
  return 0;
}
