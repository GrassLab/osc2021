# include "oneshot_timer.h"
# include "uart.h"
# include "my_math.h"
# include "my_string.h"
# include "timer.h"

char timeout_string[PRINT_TIMEOUT_STR_BUFFER_LEN][PRINT_TIMEOUT_STR_LEN];

void oneshot_timer_init(){
  for (int i=0; i<PRINT_TIMEOUT_STR_BUFFER_LEN; i++){
    timeout_string[i][0] = '\0';
  }
}
void print_timeout_string(int token){
  uart_puts(timeout_string[token]);
  uart_puts((char *) "\n");
  timeout_string[token][0] = '\0';
}

void svc_set_timeout(char *c, unsigned int sec){
  struct one_shot_timer n;
  n.ms = sec*1000;
  n.func = &print_timeout_string;
  for (int i=0; i<PRINT_TIMEOUT_STR_BUFFER_LEN; i++){
    if (timeout_string[i][0] == '\0'){
      n.token = i;
      str_copy(c, timeout_string[i]);
      break;
    }
  }
  set_timeout_svccall(&n);
}


