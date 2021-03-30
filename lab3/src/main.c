# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "shell.h"
# include "mem.h"


int main(){
  uart_init();
  uart_puts("Hi!\n");
  uart_puts("Welcome to Eric's system ~\n");
  uart_puts("(Lab3)\n");
  uart_flush();
  buddy_init();
  mem_init();
  //buddy_ll_show();

  char cmd[1000];
  cmd[0] = '\0';
  int cmd_end = 0;

  //char get_c[10];

  while(1){
    uart_puts("\r> ");
    uart_puts(cmd);
    char c = uart_read();

    // for debug
    //int_to_str((int)c, get_c);
    //uart_puts(get_c);
    //uart_puts("\n");

    if (c == '\n'){
      uart_puts("\n");
      cmd[cmd_end] = '\0';
      invoke_cmd(cmd);
      cmd_end = 0;
      cmd[0] = '\0';
    }
    else if ((int)c == 127 || (int)c == 8){
      cmd_end--;
      cmd[cmd_end] = '\0';
      uart_puts("\b \b");
    }
    else{
      cmd[cmd_end] = c;
      cmd_end++;
      cmd[cmd_end] = '\0';
    }
  }
  return 0;
}
