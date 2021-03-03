#define PERIPHERAL_BASE (0x3F000000)
#define AUX_MU_IO_REG (PERIPHERAL_BASE+0x00215040)
#define AUX_MU_LSR_REG (PERIPHERAL_BASE+0x00215054)


void init_uart();
void send_char(char c);
void send_string(char *);
char receive_char();