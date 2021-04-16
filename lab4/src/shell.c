# include "shell.h"
# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "mem.h"
# include "timer.h"
# include "svc_call.h"

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
    if (str_cmp(argv[1], (char *) "buddy") == 1){
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
      unsigned long long pct = 10;
      svc_get_core_timer_value(&pct);
      char ct[0];
      int_to_str(pct, ct);
      uart_puts((char *) "Timer value = ");
      uart_puts(ct);
      uart_puts((char *) "\n");
    }
    else if (str_cmp(argv[1], (char *) "sec")){
      unsigned long long pct = 101;
      svc_get_core_timer_ms(&pct);
      print_timer(pct, (char *) "System time = ");
    }
    else if (str_cmp(argv[1], (char *) "enable")){
      asm volatile("svc #3");
    }
    else if (str_cmp(argv[1], (char *) "disable")){
      asm volatile("svc #4");
    }
  }
  else{
    uart_puts((char *) "Command [");
    uart_puts(cmd);
    uart_puts((char *) "] not found, type \"help\" for more informations.\n");
  }
}

