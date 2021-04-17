#include "uart.h"


void uart_init(){
    register unsigned int reg;

    /* initialize UART */
    *AUX_ENABLE     |= 1;       /* enable mini UART */
    *AUX_MU_CNTL     = 0;       /* Disable transmitter and receiver during configuration. */
    *AUX_MU_IER      = 0;       /* Disable interrupt */
    *AUX_MU_LCR      = 3;       /* Set the data size to 8 bit. */
    *AUX_MU_MCR      = 0;       /* Don’t need auto flow control. */
    *AUX_MU_BAUD     = 270;     /* 115200 baud */
    *AUX_MU_IIR      = 6;       /* No FIFO */
    

    /* map UART1 to GPIO pins */
    reg = *GPFSEL1;
    reg &= ~((7<<12)|(7<<15));  /* address of gpio 14, 15 */
    reg |=   (2<<12)|(2<<15);   /* set to alt5 */
    *GPFSEL1 = reg;             /* enable gpio 14 and 15 */

    *GPPUD = 0;                 /*  disable pull-up/down */
	/*  disable need time */
    reg=150;
    while ( reg-- )
    { 
        asm volatile("nop"); 
    }
    
    *GPPUDCLK0 = (1<<14)|(1<<15);
	/*  disable need time */
    reg=150; 
    while ( reg-- )
    {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;             /* flush GPIO setup */
	
	/* initialize Mini UART */
	*AUX_ENABLE |= 1;		   // enable mini UART
	*AUX_MU_CNTL = 0;  // disable transmitter, receiver during configuration

	#ifdef	async
		*AUX_MU_IER = 1;   // enable receive interrupts
	#endif
	
	*AUX_MU_LCR = 3;   // enable 8 bit mode
	*AUX_MU_MCR = 0;   // set RTS line to be always high
	*AUX_MU_BAUD = 270;    // set baud rate to 115200
	// comment this line to avoid weird character
	// *AUX_MU_IIR_REG = 0xc6;	// no FIFO
	*AUX_MU_CNTL = 3;  // enable transmitter and receiver back


	read_buf_start = read_buf_end = 0;
	write_buf_start = write_buf_end = 0;
	enable_uart_interrupt();



}





void uart_send(char c) {
    do {
        asm volatile("nop");
    } while( ! ( *AUX_MU_LSR&0x20 ));
    *AUX_MU_IO = c;
}


char uart_get(){
    char r;
    do{        
        asm volatile("nop");        
    } while ( ! ( *AUX_MU_LSR&0x01 ) );
    r = ( char )( *AUX_MU_IO );
    return r;
}


void uart_puts(char *s){
    while(*s){
    	if(*s == '\n') uart_send('\r');
		uart_send(*s++);
    }
}
int uart_get_int(){
    int res = 0;
    char c;
    while(1){
        c = uart_get();
        if(c == '\0' || c == '\n')
            break;
        uart_send(c);
        res = res * 10 + (c - '0');
    }
    return res;
}



void uart_put_int(unsigned long num){
    if(num == 0) uart_send('0');
    else{
        if(num >= 10) uart_put_int(num / 10);
        uart_send(num % 10 + '0');
    }
}


void uart_puts_bySize(char *s, int size){
    for(int i = 0; i < size ;++i){
        if(*s == '\n') uart_send('\r');
        uart_send(*s++);
    }
}

void uart_put_hex(unsigned long d) {
    uart_puts("0x");
    unsigned long n;
    int c;
    for(c=28;c>=0;c-=4) {
        // get highest tetrad
        n=(d>>c)&0xF;
        // 0-9 => '0'-'9', 10-15 => 'A'-'F'
        n+=n>9?0x37:0x30;
        uart_send(n);
    }
}

void enable_uart_interrupt() { *ENB_IRQS1 = AUX_IRQ; }

void disable_uart_interrupt() { *DISABLE_IRQS1 = AUX_IRQ; }

void assert_transmit_interrupt() { *AUX_MU_IER |= 0x2; }

void clear_transmit_interrupt() { *AUX_MU_IER &= ~(0x2); }

void uart_handler() {
  disable_uart_interrupt();
  int rx = (*AUX_MU_IIR & 0x4);
  int tx = (*AUX_MU_IIR & 0x2);
  if (rx) {
    char c = (char)(*AUX_MU_IO);
    read_buf[read_buf_end++] = c;
    if (read_buf_end == MAX_BUFFER_LEN) read_buf_end = 0;
  } 
  else if (tx) {
    while (*AUX_MU_LSR & 0x20) {
      if (write_buf_start == write_buf_end) {
        clear_transmit_interrupt();
        break;
      }
      char c = write_buf[write_buf_start++];
      *AUX_MU_IO = c;
      if (write_buf_start == MAX_BUFFER_LEN) write_buf_start = 0;
    }
  }
  enable_uart_interrupt();
}

char uart_async_getc() {
  // wait until there are new data
  while (read_buf_start == read_buf_end) {
    asm volatile("nop");
  }
  char c = read_buf[read_buf_start++];
  if (read_buf_start == MAX_BUFFER_LEN) read_buf_start = 0;
  // '\r' => '\n'
  return c == '\r' ? '\n' : c;
}

void uart_async_puts(char *str) {
  for (int i = 0; str[i]; i++) {
    if (str[i] == '\r') write_buf[write_buf_end++] = '\n';
    write_buf[write_buf_end++] = str[i];
    if (write_buf_end == MAX_BUFFER_LEN) write_buf_end = 0;
  }
  //uart_puts(write_buf);
  assert_transmit_interrupt();
}


void test_uart_async(){
	/*  need time */
    int reg=1500;
    while ( reg-- )
    { 
        asm volatile("nop"); 
    }

	uart_async_puts("test\r\n");
}

