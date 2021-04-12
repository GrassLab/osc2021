# include "exception.h"
# include "my_math.h"
# include "uart.h"

void general_exception_handler(unsigned long arg, unsigned long type, unsigned long esr, unsigned long elr){
  char ct[20];
  uart_puts("Enter exception handler\n");
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
  switch(type){
    case 0:
    case 4:
    case 8:
    case 12:
      uart_puts("[EXCEPTION] TYPE = SYNC\n");
      sync_handler(arg, type, esr);
      break;
    default:
      break;
  }
}


void sync_handler(unsigned long arg, unsigned long type, unsigned long esr){
  char ct[20];
  uart_puts("[SYNC] EC = ");
  int ec = (esr >> 26) & 0b111111;
  int iss = esr & 0x1FFFFFF;
  int_to_hex(esr, ct);
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
    default:
      uart_puts("[SVC] unknown SVC number : ");
      int_to_hex(iss, ct);
      uart_puts(ct);
      uart_puts("\n");
      break;
  }
}
