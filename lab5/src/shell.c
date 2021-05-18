# include "shell.h"
# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "mem.h"
# include "timer.h"
# include "oneshot_timer.h"
# include "schedule.h"
# include "demo.h"
# include "user_demo.h"
# include "user_lib.h"

char *argv[SHELL_MAX_ARGC];

inline bool is_blank(char c){
  return (c == ' ') || (c == '\n') || (c == '\t');
}


char* break_char(char *c){
  char *r = c;
  while(*r){
    r++;
    if(is_blank(*r)){
      *r = '\0';
      r++;
      break;
    }
  }
  while(is_blank(*r)) r++;
  return r;
}
int cal_argc(char *cmd){
  int r = 0;
  if(*cmd == '\0'){
    argv[0] = 0;
    return r;
  }
  char *h = cmd;
  while(*h){
    argv[r] = h;
    r++;
    h = break_char(h);
  }
  return r;
}

void invoke_cmd(char *cmd){
  int argc = cal_argc(cmd);
  if (cmd[0] == '\0') return;
  if (str_cmp(argv[0], (char *) "hello") == 1){
    uart_puts((char *) "Hello World!\n");
  }
  else if (str_cmp(argv[0], (char *) "help") == 1){
    if (argc == 1){
      show_file((char *) "help/default");
    }
    else if (str_cmp(argv[1], (char *) "buddy") == 1){
      show_file((char *) "help/buddy");
    }
    else if (str_cmp(argv[1], (char *) "dma") == 1){
      show_file((char *) "help/dma");
    }
    else if (str_cmp(argv[1], (char *) "timer") == 1){
      show_file((char *) "help/timer");
    }
    else{
      show_file((char *) "help/default");
    }
  }
  else if (str_cmp(argv[0], (char *) "reboot") == 1){
    uart_puts((char *) "Rebooting ...\n");
    reset();
    while(1);
  }
  else if (str_cmp(argv[0], (char *) "ls") == 1){
    list();
  }
  else if (str_cmp(argv[0], (char *) "buddy") == 1){
    if (str_cmp(argv[1], (char *) "table")) buddy_table_show();
    else if (str_cmp(argv[1], (char *) "status")) buddy_ll_show();
  }
  else if (str_cmp(argv[0], (char *) "alloc") == 1){
    if (!argv[1]) return ;
    int alloc_size = str_to_int(argv[1]);
    char ct[20];
    unsigned long long r = (unsigned long long)malloc(alloc_size);
    if (r){
      uart_puts((char *) "\nGet memory <");
      int_to_hex(r, ct);
      uart_puts(ct);
      uart_puts((char *) ">\n");
    }
    else{
      uart_puts((char *) "\nMemory alloc fail.\n");
    }
  }
  else if (str_cmp(cmd, (char *) "free") == 1){
    if(argv[1]){
      char *addr_c = argv[1];
      if(addr_c[0] == '0' && (addr_c[1] == 'x' || addr_c[1] == 'X')){
        addr_c = addr_c+2;
      }
      unsigned long long addr = hex_to_uint(addr_c, str_len(addr_c));
      free((void *)addr);
    }
  }
  else if (str_cmp(cmd, (char *) "mem") == 1){
    if (str_cmp(argv[1], (char *) "status")) mem_ll_show();
    else uart_puts((char *) "Use \"mem status\"");
  }
  else if (str_cmp(cmd, (char *) "dma") == 1){
    if (str_cmp(argv[1], (char *) "status")) buddy_dma_ll_show();
    else uart_puts((char *) "Use \"dma status\"");
  }
  else if (str_cmp(argv[0], (char *) "cat") == 1){
    show_file(argv[1]);
  }
  else if (str_cmp(argv[0], (char *) "exec") == 1){
    exec_app(argv[1]);
  }
  else if (str_cmp(argv[0], (char *) "svc") == 1){
    asm volatile("svc #10");
  }
  else if (str_cmp(argv[0], (char *) "setTimeout") == 1){
    if (argc == 3){
      unsigned int sec = str_to_int(argv[2]);
      svc_set_timeout(argv[1], sec);
    }
  }
  else if (str_cmp(argv[0], (char *) "timer") == 1){
    if (str_cmp(argv[1], (char *) "value")){
      unsigned long long pct = svc_get_core_timer_value();
      char ct[0];
      int_to_str(pct, ct);
      uart_puts((char *) "Timer value = ");
      uart_puts(ct);
      uart_puts((char *) "\n");
    }
    else if (str_cmp(argv[1], (char *) "sec")){
      unsigned long long pct = svc_get_core_timer_ms();
      print_timer(pct, (char *) "System time = ");
    }
    else if (str_cmp(argv[1], (char *) "enable")){
      asm volatile("svc #3");
    }
    else if (str_cmp(argv[1], (char *) "disable")){
      asm volatile("svc #4");
    }
  }
  else if (str_cmp(argv[0], (char *) "demo") == 1){
    if (str_cmp(argv[1], (char *) "task1")){
      privilege_task_create(task_demo_1, 3);
      yield();
    }
    else if (str_cmp(argv[1], (char *) "task2")){
      privilege_task_create(task_demo_2, 3);
      yield();
    }
    else if (str_cmp(argv[1], (char *) "user1")){
      user_task_create(user_demo_test, 3);
      yield();
    }
  }
  else{
    uart_puts((char *) "Command [");
    uart_puts(cmd);
    uart_puts((char *) "] not found, type \"help\" for more informations.\n");
  }
}

void shell(){
  char cmd[1000];
  cmd[0] = '\0';
  int cmd_end = 0;

  //char get_c[10];

  while(1){
    uart_puts((char *) "\r> ");
    uart_puts(cmd);
    char c = uart_read();

    // for debug
    //int_to_str((int)c, get_c);
    //uart_puts(get_c);
    //uart_puts("\n");

    if (c == '\n'){
      uart_puts((char *) "\n");
      cmd[cmd_end] = '\0';
      invoke_cmd(cmd);
      cmd_end = 0;
      cmd[0] = '\0';
    }
    else if ((int)c == 127 || (int)c == 8){
      cmd_end--;
      cmd[cmd_end] = '\0';
      uart_puts((char *) "\b \b");
    }
    else{
      cmd[cmd_end] = c;
      cmd_end++;
      cmd[cmd_end] = '\0';
    }
  }
}
