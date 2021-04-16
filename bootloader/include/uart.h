void uart_init();
void uart_send(unsigned int c);
char uart_getc_boot();
char uart_getc();
void uart_puts(char *s);
void uart_read_line(char *input, int show);