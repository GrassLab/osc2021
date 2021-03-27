void uart_init();
void uart_write(unsigned int c);
char uart_read();

void uart_printf(char* fmt, ...);
void uart_flush();
