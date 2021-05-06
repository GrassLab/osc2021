# include "demo.h"
# include "schedule.h"
# include "uart.h"
# include "my_string.h"
# include "my_math.h"
# include "utli.h"
# include "exception.h"

void task_demo_1(){
  for (int i=0; i<5; i++){
    task_create(task_yield, 4);
  }
  task_exit();
}


void task_demo_2(){
  for (int i=0; i<5; i++){
    task_create(task_occupy, 4);
  }
  task_exit();
}

void task_route(int i){
    char ct[20];
    int daif = get_DAIF();
    uart_puts((char *) "Thraed PID : ");
    int_to_str(get_pid(), ct);
    uart_puts(ct);
    uart_puts((char *) " i : ");
    int_to_str(i, ct);
    uart_puts(ct);
    uart_puts((char *) " daif : ");
    int_to_hex(daif, ct);
    uart_puts(ct);
    uart_puts((char *) "\n");
    delay(50000000);
}

void task_yield(){
  IRQ_DISABLE();
  for (int i=0; i<10; i++){
    task_route(i);
    yield();
  }
  task_exit();
}

void task_occupy(){
  IRQ_ENABLE();
  for (int i=0; i<10; i++){
    task_route(i);
  }
  task_exit();
}
