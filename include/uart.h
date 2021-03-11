void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_puts(char *s);
void uart_read_line(char *input, int show);
int uart_read_int(char *input);