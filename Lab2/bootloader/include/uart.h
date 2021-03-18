void uart_init();
void uart_read_line(char *input, int show);
void uart_write(unsigned int c);
char uart_read();
void uart_printf(char* fmt, ...);
void uart_flush();
