# include "log.h"
# include "uart.h"

//enum log_type global_log_state = WARNING;
enum log_type global_log_state = INFO;

void log_puts(const char *s, enum log_type level){
  if (level <= global_log_state){
    uart_puts(s);
  }
}

void set_log_level(enum log_type level){
  global_log_state = level;
}

enum log_type get_log_level(){
  return global_log_state;
}
