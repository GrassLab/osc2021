#define MMIO_BASE       0x3F000000
#define AUX_ENABLE      ((volatile unsigned int*)(MMIO_BASE+0x00215004))
#define AUX_MU_IO       ((volatile unsigned int*)(MMIO_BASE+0x00215040))
#define AUX_MU_IER      ((volatile unsigned int*)(MMIO_BASE+0x00215044))
#define AUX_MU_IIR      ((volatile unsigned int*)(MMIO_BASE+0x00215048))
#define AUX_MU_LCR      ((volatile unsigned int*)(MMIO_BASE+0x0021504C))
#define AUX_MU_MCR      ((volatile unsigned int*)(MMIO_BASE+0x00215050))
#define AUX_MU_LSR      ((volatile unsigned int*)(MMIO_BASE+0x00215054))
#define AUX_MU_MSR      ((volatile unsigned int*)(MMIO_BASE+0x00215058))
#define AUX_MU_SCRATCH  ((volatile unsigned int*)(MMIO_BASE+0x0021505C))
#define AUX_MU_CNTL     ((volatile unsigned int*)(MMIO_BASE+0x00215060))
#define AUX_MU_STAT     ((volatile unsigned int*)(MMIO_BASE+0x00215064))
#define AUX_MU_BAUD     ((volatile unsigned int*)(MMIO_BASE+0x00215068))

__attribute__((section(".text.uart")))void uart_write(unsigned int c) {
    /* wait until we can send */
    do{
      asm volatile("nop");
    } while ( !( *AUX_MU_LSR&0x20 ) );
    /* write the character to the buffer */
    *AUX_MU_IO = c;
}

__attribute__((section(".text.uart"))) void uart_puts(char *s) {
    while(*s) {
        /* convert newline to carrige return + newline */
      if (*s == '\n') uart_write('\r');
      uart_write(*s++);	    
    }
}
__attribute__((section(".text.uart"))) char uart_read() {
    char r;
    /* wait until something is in the buffer */
    do{
      asm volatile("nop");
    } while ( !( *AUX_MU_LSR&0x01 ) );
    /* read it and return */
    r = (char)(*AUX_MU_IO);
    /* convert carrige return to newline */
    return (r == '\r') ? '\n' : r;
}
__attribute__((section(".text.app"))) void app(){
  //uart_puts(meg);
  char ca[4];
  ca[0] = 'H';
  ca[1] = 'i';
  ca[2] = '\n';
  ca[3] = '\0';
  uart_puts(ca);
  //char r = uart_read();
  //uart_puts(meg);
  //return (int)r;
}
