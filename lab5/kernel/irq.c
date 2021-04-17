#include "irq.h"
#include <printf.h>
#include <interrupt.h>
#include <uart.h>
#include <timer.h>
#include <string.h>
#include <sched.h>

void irq_routing() {
  //printf("CORE0_INTERRUPT_SOURCE: %d\n", *CORE0_INTERRUPT_SOURCE);
  if(*CORE0_INTERRUPT_SOURCE & 0x2) {
    //nCNTHPIRQ core 0 interrupt source 2
    // core timer interrupt handler
    core_time_interrupt_handler();
    schedule();
  }
  else if(*CORE0_INTERRUPT_SOURCE & (1 << 8)) {
    //GPU interrupt
    if(*UART_IRQ_BASIC_PENDING & (1 << 8)) {
      //pending1 enable
      if(*UART_IRQ_PENDING1 & (1 << 29)) {
        //mini UART interrupt
        irq_uart_handler();
      }
    }
  }
}

void irq_uart_handler() {
  if(*AUX_MU_IIR_REG & 4) {
    //[2:1] 10: receiver holds valid byte
    //rx interrupt
    char c; 
    if(!circular_queue_is_full(&uart_read_buffer)) {
      c = (char)(*AUX_MU_IO_REG);
      c = c=='\r'?'\n':c;
      if(c == '\n') {
        uart_rx_interrupt_disable();
        circular_queue_push(&uart_read_buffer, '\r'); 
      }
      circular_queue_push(&uart_read_buffer, c); 
      
    }
    else
      uart_rx_interrupt_disable();
  }
  else if(*AUX_MU_IIR_REG & 2) {
    // [2:1] 01: transmit holding register empty
    //tx interrupt
    if(!circular_queue_is_empty(&uart_write_buffer))
      *AUX_MU_IO_REG = circular_queue_pop(&uart_write_buffer);
    else 
      uart_tx_interrupt_disable();
  }
}