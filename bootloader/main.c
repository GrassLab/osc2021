# include "uart.h"
# include "utli.h"
# include "gpio.h"
# include "my_math.h"
# include "my_string.h"
# include "relocate.h"

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


int main(){
  uart_init();
  uart_puts("Hi!\n");
  uart_puts("Welcome to Eric's system ~\n");
  uart_puts("(Bootloader)\n");
  uart_flush();

  char cmd[1000];
  cmd[0] = '\0';
  int cmd_end = 0;

  while(1){
    uart_puts("\r> ");
    uart_puts(cmd);
    char c = uart_read();

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
