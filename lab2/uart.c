#include "utils.h"

#define PBASE 0x3F000000

#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

void uart_init ( void )
{
    unsigned int selector;

    selector = get32(GPFSEL1);
    selector &= ~(7<<12);                   // clean gpio14
    selector |= 2<<12;                      // set alt5 for gpio14
    selector &= ~(7<<15);                   // clean gpio15
    selector |= 2<<15;                      // set alt5 for gpio 15
    put32(GPFSEL1,selector);

    put32(GPPUD,0);
    delay(150);
    put32(GPPUDCLK0,(1<<14)|(1<<15));
    delay(150);
    put32(GPPUDCLK0,0);

    put32(AUX_ENABLES,1);                   //Enable mini uart (this also enables access to its registers)
    put32(AUX_MU_CNTL_REG,0);               //Disable auto flow control and disable receiver and transmitter (for now)
    put32(AUX_MU_IER_REG,0);                //Disable receive and transmit interrupts
    put32(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
    put32(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
    put32(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200

    put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}

void uart_send ( char c )
{
    while(1) {
        if(get32(AUX_MU_LSR_REG)&0x20)      // 5th bit is set if the transmit FIFO can accept at least one byte.
            break;
    }
    put32(AUX_MU_IO_REG,c);
}

char uart_recv ( void )
{
    while(1) {
        if(get32(AUX_MU_LSR_REG)&0x01)      // 1sts bit is set if the receive FIFO holds at least 1 symbol.
            break;
    }
    return(get32(AUX_MU_IO_REG)&0xFF);
}

void uart_send_string(char* str)
{
    for (int i = 0; str[i] != '\0'; i ++) {
        uart_send((char)str[i]);
    }
}

int read_line(char buf[], int buf_size)
{
    char *buf_ptr = buf;
    char ch;
    int cnt = 0;

    while ((ch = uart_recv()) != '\r' && ++cnt < buf_size) {
        uart_send(ch);
        *buf_ptr++ = ch;
    }
    uart_send_string("\r\n");

    *buf_ptr = '\0';
    return cnt;
}


int uart_read_int() {
    int num = 0, index, i;
    char buff[100];
    index = read_line(buff, 100);
    for (i = 0 ; i < index; i++) {
        num = num * 10;
        num += buff[i] - '0';
    }
    return num;
}

void reverse(char *str,int index)
{
    int i = 0, j = index - 1, temp;
    while (i < j) {
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp;
        i++;
        j--;
    }
}

void uart_send_int(int number) {
    int i = 0, j; 
    char str[100];
    while (number) { 
        str[i++] = (number % 10) + '0'; 
        number = number / 10; 
    } 
    reverse(str, i);
    for ( j = 0 ; j < i ; j++) {
        uart_send(str[j]);
    }
    // uart_send('\r\n');
    return ; 
}

void uart_send_long(long number) {
    int i = 0, j; 
    char str[100];
    while (number) { 
        str[i++] = (number % 10) + '0'; 
        number = number / 10; 
    } 
    reverse(str, i);
    for ( j = 0 ; j < i ; j++) {
        uart_send(str[j]);
    }
    // uart_send('\r\n');
    return ; 
}