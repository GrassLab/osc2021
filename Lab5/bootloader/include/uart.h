void uart_init();

void uart_read_line_real(char *input, int show);
void uart_write(unsigned int c);
char uart_read();
char uart_read_real();
void uart_printf(char* fmt, ...);
void uart_flush();
