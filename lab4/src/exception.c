# include "exception.h"
# include "my_math.h"
# include "uart.h"
# include "mem_addr.h"
# include "timer.h"

void general_exception_handler(unsigned long arg, unsigned long type, unsigned long esr, unsigned long elr){
  char ct[20];
  switch(type){
    case 0:
    case 4:
    case 8:
    case 12:
      sync_handler(arg, type, esr, elr);
      break;
    case 1:
    case 5:
    case 9:
    case 13:
      irq_handler();
    default:
      break;
  }
}


void sync_handler(unsigned long arg, unsigned long type, unsigned long esr, unsigned long elr){
  char ct[20];
  uart_puts("Enter SYNC handler\n");
  uart_puts("[EXCEPTION] TYPE = ");
  int_to_str(type, ct);
  uart_puts(ct);
  uart_puts("\t");
  uart_puts("Desc : ");
  uart_puts(vector_table_desc[type]);

  uart_puts("\n[EXCEPTION] ESR = ");
  int_to_hex(esr, ct);
  uart_puts(ct);
  uart_puts("\t");
  uart_puts("ELR = ");
  int_to_hex(elr, ct);
  uart_puts(ct);
  uart_puts("\n");
  uart_puts("[SYNC] EC = ");
  int ec = (esr >> 26) & 0b111111;
  int iss = esr & 0x1FFFFFF;
  int_to_hex(ec, ct);
  uart_puts(ct);
  uart_puts(", ISS = ");
  int_to_hex(iss, ct);
  uart_puts(ct);
  uart_puts("\n");
  // check if SVC
  if (ec == ESR_EC_SVC){
    uart_puts("[SYNC] Call SVC handler\n");
    svc_handler(arg, type, iss);
    return ;
  }
}


void svc_handler(unsigned long arg, unsigned long type, int iss){
  char ct[20];
  switch(iss){
    case SVC_ISS_NOPE:
      uart_puts("[SVC] inside SVC handler\n");
      break;
    case SVC_ISS_GET_TIMER_VALUE:
      get_core_timer_value((unsigned long long *)arg);
      break;
    case SVC_ISS_GET_TIMER_MS:
      get_core_timer_ms((unsigned long long *)arg);
      break;
    case SVC_ISS_PRINT_SYSTEM_TIME_ENABLE:
      print_system_time_enable();
      break;
    case SVC_ISS_PRINT_SYSTEM_TIME_DISABLE:
      print_system_time_disable();
      break;
    case SVC_ISS_SET_ONE_SHOT_TIMER:
      set_one_shot_timer((struct one_shot_timer *)arg);
      break;
    default:
      uart_puts("[SVC] unknown SVC number : ");
      int_to_hex(iss, ct);
      uart_puts(ct);
      uart_puts("\n");
      break;
  }
  uart_puts("return\n");
}

void irq_handler(){
  unsigned int irq_basic_pending = *((unsigned int *) IRQ_BASIC_PENDING);
  unsigned int core0_intr_src = *((unsigned int *) CORE0_INTR_SRC);
  if (core0_intr_src & (1 << 1)){
    core_timer_interrupt_handler();
  }
}
