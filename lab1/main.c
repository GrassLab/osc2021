# include "uart.h"
# include "utli.h"

//itoa
void int_to_str(int n, char *s){
  char tmp[100];
  int idx = 0;
  do{
    tmp[idx] = (char)((n%10) + 48);
    idx++;
    n /= 10;
  } while(n > 0);
  for (int i=0; i<idx; i++){
    s[i] = tmp[idx-i-1];
  }
  s[idx] = '\0';
}

//str compare
int str_cmp(char *s1, char *s2){
  int i = 0;
  if (s1[0] == '\0' && s2[0] == '\0') return 1;
  while(s1[i]){
    if (s1[i] != s2[i]) return 0;
    i++;
  }

  if (s2[i] == '\0') return 1;
  return 0;
}

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
    uart_puts("reboot  | reboot pi\n");
  }
  else if (str_cmp(cmd, "reboot") == 1){
    uart_puts("Rebooting ...\n");
    reset();
    while(1);
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
  uart_puts("(Lab1)\n");
  uart_flush();
  uart_puts("> ");

  char cmd[1000];
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
