# include "shell.h"
# include "uart.h"
# include "utli.h"
# include "cpio.h"
# include "my_math.h"
# include "my_string.h"
# include "buddy.h"
# include "mem.h"

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
  if (str_cmp(argv[0], "hello") == 1){
    uart_puts("Hello World!\n");
  }
  else if (str_cmp(argv[0], "help") == 1){
    if (str_cmp(argv[1], "buddy") == 1){
      show_file("help/buddy");
    }
    if (str_cmp(argv[1], "dma") == 1){
      show_file("help/dma");
    }
    else{
      show_file("help/default");
    }
    /*
    uart_puts("Command | Description\n");
    uart_puts("--------| ----------------------------\n");
    uart_puts("hello   | print Hello World!\n");
    uart_puts("help    | print all available commands\n");
    uart_puts("reboot  | reboot pi\n");
    */
  }
  else if (str_cmp(argv[0], "reboot") == 1){
    uart_puts("Rebooting ...\n");
    reset();
    while(1);
  }
  else if (str_cmp(argv[0], "ls") == 1){
    list();
  }
  else if (str_cmp(argv[0], "buddy") == 1){
    if (str_cmp(argv[1], "table")) buddy_table_show();
    else if (str_cmp(argv[1], "status")) buddy_ll_show();
  }
  else if (str_cmp(argv[0], "alloc") == 1){
    if (!argv[1]) return ;
    int alloc_size = str_to_int(argv[1]);
    char ct[20];
    unsigned long long r = (unsigned long long)malloc(alloc_size);
    if (r){
      uart_puts("\nGet memery <");
      int_to_hex(r, ct);
      uart_puts(ct);
      uart_puts(">\n");
    }
    else{
      uart_puts("\nMemery alloc fail.\n");
    }
  }
  else if (str_cmp(cmd, "free") == 1){
    if(argv[1]){
      char *addr_c = argv[1];
      if(addr_c[0] == '0' && (addr_c[1] == 'x' || addr_c[1] == 'X')){
        addr_c = addr_c+2;
      }
      unsigned long long addr = hex_to_uint(addr_c, str_len(addr_c));
      free((void *)addr);
    }
  }
  else if (str_cmp(cmd, "mem") == 1){
    if (str_cmp(argv[1], "status")) mem_ll_show();
    else uart_puts("Use \"mem status\"");
  }
  else if (str_cmp(cmd, "dma") == 1){
    if (str_cmp(argv[1], "status")) buddy_dma_ll_show();
    else uart_puts("Use \"dma status\"");
  }
  else if (str_cmp(argv[0], "cat") == 1){
    show_file(argv[1]);
  }
  else{
    uart_puts("Command [");
    uart_puts(cmd);
    uart_puts("] not found, type \"help\" for more informations.\n");
  }
}

