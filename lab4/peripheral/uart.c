#include "uart.h"
#include <interrupt.h>
#include <string.h>
void uart_init() {

  register unsigned int r;

  /* map UART1 to GPIO pins */
  r=*GPFSEL1;
  r&=~((7<<12)|(7<<15)); // gpio14, gpio15
  r|=(2<<12)|(2<<15);    // alt5
  *GPFSEL1 = r;
  *GPPUD = 0;            // enable pins 14 and 15
  r=150; while(r--) { asm volatile("nop"); }
  *GPPUDCLK0 = (1<<14)|(1<<15);
  r=150; while(r--) { asm volatile("nop"); }
  *GPPUDCLK0 = 0;        // flush GPIO setup
  
  *AUX_ENABLES |= 1;
  *AUX_MU_CNTL_REG = 0;
  *AUX_MU_LCR_REG = 3;
  *AUX_MU_MCR_REG = 0;
  *AUX_MU_IER_REG = 0;
  *AUX_MU_IIR_REG = 6;
  *AUX_MU_BAUD_REG = 270;
  *AUX_MU_CNTL_REG = 3;
  
  //routing interrupt to core 0
  *GPU_INTERRUPT_ROUTING = 0;
  
  //*AUX_MU_IER_REG = 3;
  *UART_IRQ1_ENABLE = (1 << 29);
}

/**
 * Send a character
 */
void uart_send(unsigned int c) {
  /* wait until we can send */
  //AUX_MU_LSR's 5th bit is to check if we can send or not
  do{asm volatile("nop");}while(!(*AUX_MU_LSR_REG&0x20));
  /* write the character to the buffer */
  *AUX_MU_IO_REG=c;
}
/**
 * Receive a character
 */
char uart_getc() {
  char r;
  /* wait until something is in the buffer */
  do{asm volatile("nop");}while(!(*AUX_MU_LSR_REG&0x01));
  /* read it and return */
  r=(char)(*AUX_MU_IO_REG);
  return r;
}

/**
 *  * Display a string
 *   */
void uart_puts(char *s) {
  while(*s) {
  /* convert newline to carrige return + newline */
    if(*s=='\n') 
      uart_send('\r');
    uart_send(*s++);
  }
}

void uart_hex(unsigned int d) {
  unsigned int n;
  int c;
  for (c = 28; c >= 0; c -= 4)
    {
      // get highest tetrad
      n = (d >> c) & 0xF;
      // 0-9 => '0'-'9', 10-15 => 'A'-'F'
      n += n > 9 ? 0x37 : 0x30;
      uart_send (n);
    }
}

size_t uart_read(char* buf, size_t count) {
  size_t i;
  for(i = 0; i < count; i++) {
    *buf = uart_getc();
    buf++;
  }
  return i;
}

size_t uart_readline(char* buf, size_t count) {
  size_t i;
  char c;
  for(i = 0; i < count; i++) {
    c = uart_getc();
    if(c =='\n' || c == '\r') {
      uart_send('\r');
      uart_send('\n');
      i++;
      break;
    } 
    uart_send(c);
    buf[i] = c;
  }
  buf[i] = '\0';
  return i;
}

size_t uart_write(char *buf, size_t count) {
  size_t i;
  for(i = 0; i < count; i++) {
    if(*buf =='\n') 
      uart_send('\r');
    uart_send(*buf++);
  }
  return i;
}

void putc( void* p, char c) {
	if(c =='\n') 
    uart_send('\r');
  uart_send(c);
}

void uart_tx_interrupt_enable() {
  *AUX_MU_IER_REG |= 2;
}

void uart_rx_interrupt_enable() {
  *AUX_MU_IER_REG |= 1;
}

void uart_tx_interrupt_disable() {
  *AUX_MU_IER_REG &= ~2;
}

void uart_rx_interrupt_disable() {
  //*UART_IRQ1_DISABLE = (1 << 29);
  *AUX_MU_IER_REG &= ~1;
}

size_t uart_async_write(char* buf, size_t count) {
  //write to write buffer
  for(int i = 0; i < count; i++) {
    if(buf[i] == '\n') 
      circular_queue_push(&uart_write_buffer, '\r');
    circular_queue_push(&uart_write_buffer, buf[i]);
  }
  //enable tx interrupt
  uart_tx_interrupt_enable();
  return count;
}

size_t uart_async_read(char* buf, size_t size) {
  size_t count;
  uart_rx_interrupt_enable();
  while((*AUX_MU_IER_REG) & 1);
  count = 0;
  char c;
  //circular_queue_status(&uart_read_buffer);
  while(!circular_queue_is_empty(&uart_read_buffer) && count < size) {
    c = circular_queue_pop(&uart_read_buffer);
    buf[count] = c;
    //uart_send(c);
    count++;
  }
  if(count < size)
    buf[count] = '\0';
  return count;
}
