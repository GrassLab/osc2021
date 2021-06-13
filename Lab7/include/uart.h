void uart_init();
void uart_write(unsigned int c);
char uart_read();
int uart_gets(char* s,int size,int display);
void uart_puts(char *s);
unsigned long uart_getX(int display);
void uart_printf(char* fmt, ...);
void uart_flush();
