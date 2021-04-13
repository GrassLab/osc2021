# include "timer.h"
# include "uart.h"
# include "my_math.h"

unsigned long long core_timer_peroid;  // unit : ms

void print_timer(unsigned long long ms){
  uart_puts("System time = ");
  char ct[20];
  int_to_str(ms/1000, ct);
  uart_puts(ct);
  uart_puts(".");
  unsigned int demical= ms%1000;
  for (int i=0; i<1; i++){
    if (demical/100 == 0){
      uart_puts("0");
    }
    else{
      break;
    }
    if (demical/10 == 0){
      uart_puts("0");
    }
    else{
      break;
    }
  }
  int_to_str(ms%1000, ct);
  uart_puts(ct);
  uart_puts(" s\n");
}

extern "C"
void core_timer_init(){
  register unsigned int enable = 1;
  asm volatile("msr cntp_ctl_el0, %0" : : "r"(enable));
  core_timer_peroid = CORE_TIMER_DEFAULT_FREQ;
}

void set_next_tval(){
  char ct[20];
  register unsigned long long tval;
  asm volatile("mrs %0, cntfrq_el0" : "=r"(tval));
  //int_to_hex(tval, ct);
  //uart_puts(ct);
  //uart_puts("\t");
  tval = tval*core_timer_peroid/1000;
  //int_to_hex(tval, ct);
  //uart_puts(ct);
  //uart_puts("\n");
  asm volatile("msr cntp_tval_el0, %0" : : "r"(tval));
}

void core_timer_enable(){
  *((unsigned int *)CORE0_TIMER_IRQ_CTRL) |= 1 << 1;
  set_next_tval();
}

void core_timer_disable(){
  // disable timer interrupt
  *((unsigned int *)CORE0_TIMER_IRQ_CTRL) &= !(1 << 1);
}

void core_timer_interrupt_handler(){
  register unsigned long long ms;
  get_core_timer_ms(&ms);
  print_timer(ms);
  set_next_tval();
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
