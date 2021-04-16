#include "dtp.h"

#define PBASE 0x3F000000

#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044) //p.12
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

void uart_init ( void );
int uart_probe(struct dtn *node);
char uart_recv ( void );
void uart_send ( char c );
void uart_send_async ( char c );
void uart_send_string(char* str);
void uart_send_string_async(char* str);
void uart_send_string_low_power(char* str);
int read_line(char buf[], int buf_size);
int read_line_low_power(char buf[], int buf_size);
int uart_read_int(void);
void reverse(char *str,int index);
void uart_send_int(int number);
void uart_send_uint(unsigned int number);
void uart_send_long(long number);
void uart_send_ulong(unsigned long number);

