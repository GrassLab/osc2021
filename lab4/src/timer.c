# include "timer.h"
# include "uart.h"
# include "my_math.h"
# include "linklist.c"

unsigned long long core_timer_freq;
unsigned long long print_timer_peroid;  // unit : ms
struct timer_object timer_object_list[MAX_TIMER_QUEUE_SIZE];
struct timer_object *timer_inuse;
struct timer_object *timer_unuse;

int print_system_time_flag = 0;

void print_timer(unsigned long long ms, char *comment){
  uart_puts(comment);
  char ct[20];
  int_to_str(ms/1000, ct);
  uart_puts(ct);
  uart_puts((char *) ".");
  unsigned int demical= ms%1000;
  for (int i=0; i<1; i++){
    if (demical/100 == 0){
      uart_puts((char *) "0");
    }
    else{
      break;
    }
    if (demical/10 == 0){
      uart_puts((char *) "0");
    }
    else{
      break;
    }
  }
  int_to_str(ms%1000, ct);
  uart_puts(ct);
  uart_puts((char *) " s\n");
}

extern "C"
void core_timer_init(){
  register unsigned int enable = 1;
  asm volatile("msr cntp_ctl_el0, %0" : : "r"(enable));
  register unsigned long long tval;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(tval));
  core_timer_freq = tval;
  print_timer_peroid = PRINT_TIMER_DEFAULT_FREQ;
  for (int i=0; i<MAX_TIMER_QUEUE_SIZE; i++){
    timer_object_list[i].target_cval = 0;
    timer_object_list[i].reg_cval = 0;
    timer_object_list[i].func = 0;
    timer_object_list[i].pre = (i == 0) ? 0 : &timer_object_list[i-1];
    timer_object_list[i].next = (i == MAX_TIMER_QUEUE_SIZE-1) ? 0 : &timer_object_list[i+1];
  }
  timer_unuse = &timer_object_list[0];
  timer_inuse = 0;
}

void timer_queue_insert(unsigned long long tval, int token, void (*func)(int)){
  core_timer_disable();
  struct timer_object *new_node = ll_pop_front<struct timer_object>(&timer_unuse);
  unsigned long long t;
  get_core_timer_value(&t);
  new_node->reg_cval = t;
  t += tval;
  new_node->target_cval = t;
  new_node->token = token;
  new_node->func = func;
  if (!timer_inuse){
    ll_push_front<struct timer_object>(&timer_inuse, new_node);
  }
  else{
    struct timer_object *insert_point = timer_inuse;
    while (insert_point->next && insert_point->next->target_cval < t){
      insert_point = insert_point->next;
    }
    if (insert_point->target_cval > t){
      insert_point = 0;
    }
    ll_push_elm<struct timer_object>(&timer_inuse, new_node, insert_point);
  }
  set_cval_register();
}

void set_cval_register(){
  if (!timer_inuse){
    core_timer_disable();
  }
  else{
    register unsigned long long cval = timer_inuse->target_cval;
    asm volatile("msr cntp_cval_el0, %0" : : "r"(cval));
    core_timer_enable();
  }
}


inline void core_timer_enable(){
  // enable timer iinterrupt
  *((unsigned int *)CORE0_TIMER_IRQ_CTRL) |= 1 << 1;
  //asm volatile("msr DAIFClr, 0x2");
}

inline void core_timer_disable(){
  // disable timer interrupt
  *((unsigned int *)CORE0_TIMER_IRQ_CTRL) &= ~(1 << 1);
  //asm volatile("msr DAIFSet, 0x2");
}

void print_system_time_enable(){
  print_system_time_flag = 1;
  set_next_pst_tval();
}

void print_system_time_disable(){
  print_system_time_flag = 0;
}

void core_timer_interrupt_handler(){
  struct timer_object *invoke_node = ll_pop_front<struct timer_object>(&timer_inuse);
  unsigned long long reg_cval = invoke_node->reg_cval;
  void (*invoke_func)(int) = invoke_node->func;
  int token = invoke_node->token;
  ll_push_front<struct timer_object>(&timer_unuse, invoke_node);
  set_cval_register();
  if (token < 0){
    invoke_func(token);
  }
  else{
    uart_puts((char *) "\n");
    print_timer(reg_cval/(core_timer_freq/1000), (char *) "Command executed time = ");
    unsigned long long cur_ms;
    get_core_timer_ms(&cur_ms);
    print_timer(cur_ms, (char *) "Current time = ");
    invoke_func(token);
  }
}

void get_core_timer_value(unsigned long long* r){
  register unsigned long long pct;
  asm volatile("mrs %0, cntpct_el0" : "=r"(pct));
  *r = pct;
}

void get_core_timer_ms(unsigned long long *r){
  register unsigned long long pct;
  register unsigned long long freq;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
  asm volatile("mrs %0, cntpct_el0" : "=r"(pct));
  *r = pct/(freq/1000);
}

void print_system_timer_cb(int token){
  if (print_system_time_flag){
    register unsigned long long ms;
    get_core_timer_ms(&ms);
    print_timer(ms, (char *) "System time = ");
    set_next_pst_tval();
  }
}

void set_next_pst_tval(){
  if (print_system_time_flag == 0){
    return ;
  }
  register unsigned long long tval = core_timer_freq*print_timer_peroid/1000;
  timer_queue_insert(tval, -1, &print_system_timer_cb);
}

void set_one_shot_timer(struct one_shot_timer *n){
  register unsigned long long tval = core_timer_freq*(n->ms)/1000;
  timer_queue_insert(tval, n->token, n->func);
}
