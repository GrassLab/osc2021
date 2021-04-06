void uart_init();
void uart_send(unsigned int c);
char uart_getc();
unsigned char uart_getb();
unsigned long uart_getX(int display);
void uart_puts(char *s);
void uart_puthex(unsigned int hex);
unsigned int uart_printf(char* fmt,...);
int strcmp(char* a,char* b);