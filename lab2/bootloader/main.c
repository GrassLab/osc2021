# include "uart.h"
# include "utli.h"
# include "gpio.h"

//# include "relocate.h"

extern unsigned char __stack_top, _begin, _end;

void invoke_cmd(char *cmd){
  if (cmd[0] == '\0') return;
  if (str_cmp(cmd, "hello") == 1){
    uart_puts("Hello World!\n");
  }
  else if (str_cmp(cmd, "help") == 1){
    uart_puts("Command | Description\n");
    uart_puts("--------| ----------------------------\n");
    uart_puts("hello   | print Hello World!\n");
    uart_puts("help    | print all available commands\n");
    uart_puts("loadimg | Load kernal image from UART\n");
    uart_puts("reboot  | reboot pi\n");
  }
  else if (str_cmp(cmd, "reboot") == 1){
    uart_puts("Rebooting ...\n");
    reset();
    while(1);
  }
  else if (str_cmp(cmd, "loadimg") == 1){
    loadimg();
  }
  else{
    uart_puts("Command [");
    uart_puts(cmd);
    uart_puts("] not found, type \"help\" for more informations.\n");
  }
}

/*
void show_mainf_addr(){
  char hex_c[20];
  void (*func_p)(char) = &main;
  int_to_hex(func_p, hex_c);
  uart_puts(hex_c);
  uart_puts("\n");
}
*/

int main(){
  //relocate();
  uart_init();
  uart_puts("Hi!\n");
  uart_puts("Welcome to Eric's system ~\n");
  uart_puts("(Bootloader)\n");
  uart_flush();
  char hex_c[20];
  /*
  for (unsigned long i = 0; i<20; i++){
    int_to_hex(i, hex_c);
    uart_puts(hex_c);
    uart_puts("\n");
  }
  
  int_to_hex(&__stack_top, hex_c);
  uart_puts(hex_c);
  uart_puts("\n");
  int_to_hex(&_begin, hex_c);
  uart_puts(hex_c);
  uart_puts("\n");
  int_to_hex(&_end, hex_c);
  uart_puts(hex_c);
  uart_puts("\n");
  uart_flush();
  void (*func_p)(char) = &invoke_cmd;
  int_to_hex(func_p, hex_c);
  uart_puts(hex_c);
  uart_puts("\n");
  void * cpc = __builtin_return_address(0);
  int_to_hex(cpc, hex_c);
  uart_puts("Main pc = ");
  uart_puts(hex_c);
  uart_puts("\n");
  //show_mainf_addr();
  */


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
