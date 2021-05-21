# include "exception.h"
# include "my_math.h"
# include "uart.h"
# include "mem_addr.h"
# include "timer.h"
# include "schedule.h"
# include "vfs.h"


char vector_table_desc[16][30] = {
  "0x000, Synchronous-SP_EL0",
  "0x080, IRQ-SP_EL0",
  "0x100, FIQ-SP_EL0",
  "0x180, sError-SP_EL0",
  "0x200, Synchronous-SP_ELx",
  "0x280, IRQ-SP_ELx",
  "0x300, FIQ-SP_ELx",
  "0x380, sError-SP_ELx",
  "0x400, Synchronous-AArch64",
  "0x480, IRQ-AArch64",
  "0x500, FIQ-AArch64",
  "0x580, sError-AArch64",
  "0x600, Synchronous-AArch32",
  "0x680, IRQ-AArch32",
  "0x700, FIQ-AArch32",
  "0x780, sError-AArch32",
};

void general_exception_handler(struct trapframe *arg, unsigned long type, unsigned long esr, unsigned long elr){
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
  schedule();
}


void sync_handler(struct trapframe *arg, unsigned long type, unsigned long esr, unsigned long elr){
  int ec = (esr >> 26) & 0b111111;
  int iss = esr & 0x1FFFFFF;
  // check if SVC
  if (ec == ESR_EC_SVC){
    //uart_puts((char *) "[SYNC] Call SVC handler\n");
    svc_handler(arg, type, iss);
    return ;
  }
  /*
  char ct[20];
  uart_puts((char *) "Enter SYNC handler\n");
  uart_puts((char *) "[EXCEPTION] TYPE = ");
  int_to_str(type, ct);
  uart_puts(ct);
  uart_puts((char *) "\t");
  uart_puts((char *) "Desc : ");
  uart_puts(vector_table_desc[type]);

  uart_puts((char *) "\n[EXCEPTION] ESR = ");
  int_to_hex(esr, ct);
  uart_puts(ct);
  uart_puts((char *) "\t");
  uart_puts((char *) "ELR = ");
  int_to_hex(elr, ct);
  uart_puts(ct);
  uart_puts((char *) "\n");
  uart_puts((char *) "[SYNC] EC = ");
  int_to_hex(ec, ct);
  uart_puts(ct);
  uart_puts((char *) ", ISS = ");
  int_to_hex(iss, ct);
  uart_puts(ct);
  uart_puts((char *) "\n");
  */
}


void svc_handler(struct trapframe *arg, unsigned long type, int iss){
  char ct[20];
  unsigned long long ans_ull;
  //int ans_int;
  switch(iss){
    case SVC_ISS_NOPE:
      uart_puts((char *) "[SVC] inside SVC handler\n");
      break;
    case SVC_ISS_GET_TIMER_VALUE:
      ans_ull = get_core_timer_value();
      arg->x[0] = ans_ull;
      break;
    case SVC_ISS_GET_TIMER_MS:
      ans_ull = get_core_timer_ms();
      arg->x[0] = ans_ull;
      break;
    case SVC_ISS_PRINT_SYSTEM_TIME_ENABLE:
      print_system_time_enable();
      break;
    case SVC_ISS_PRINT_SYSTEM_TIME_DISABLE:
      print_system_time_disable();
      break;
    case SVC_ISS_SET_ONE_SHOT_TIMER:
      set_one_shot_timer((struct one_shot_timer *)(arg->x[0]));
      break;
    case SVC_ISS_GETPID:
      arg->x[0] = get_pid();
      break;
    case SVC_ISS_UART_READ:
      arg->x[0] = svc_uart_read((char *)arg->x[0], (int)arg->x[1]);
      break;
    case SVC_ISS_UART_WRITE:
      arg->x[0] = svc_uart_write((char *)arg->x[0], (int)arg->x[1]);
      break;
    case SVC_ISS_EXEC:
      sys_exec(arg);
      break;
    case SVC_ISS_EXIT:
      task_exit();
      break;
    case SVC_ISS_FORK:
      sys_fork(arg);
      break;
    case SVC_ISS_OPEN:
      sys_open(arg);
      break;
    case SVC_ISS_CLOSE:
      sys_close(arg);
      break;
    case SVC_ISS_WRITE:
      sys_write(arg);
      break;
    case SVC_ISS_READ:
      sys_read(arg);
      break;
    case SVC_ISS_MKDIR:
      sys_mkdir(arg);
      break;
    case SVC_ISS_CHDIR:
      sys_chdir(arg);
      break;
    case SVC_ISS_MOUNT:
      sys_mount(arg);
      break;
    case SVC_ISS_UNMOUNT:
      sys_unmount(arg);
      break;
    default:
      uart_puts((char *) "[SVC] unknown SVC number : ");
      int_to_hex(iss, ct);
      uart_puts(ct);
      uart_puts((char *) "\n");
      break;
  }
  //uart_puts((char *) "return\n");
}

void irq_handler(){
  //unsigned int irq_basic_pending = *((unsigned int *) IRQ_BASIC_PENDING);
  unsigned int core0_intr_src = *((unsigned int *) CORE0_INTR_SRC);
  if (core0_intr_src & (1 << 1)){
    core_timer_interrupt_handler();
  }
  IRQ_ENABLE();
}

int get_DAIF(){
  register unsigned long long daif;
  asm volatile ("mrs %0, DAIF" : "=r"(daif));
  return daif;
}
