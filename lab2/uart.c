#include "utils.h"
#include "include/dtp.h"
#include "include/cutils.h"
#include "include/mini_uart.h"


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
    put32(AUX_MU_IER_REG,1);                //Enable only receive interrupts
    put32(AUX_MU_LCR_REG,3);                //Enable 8 bit mode
    put32(AUX_MU_MCR_REG,0);                //Set RTS line to be always high
    put32(AUX_MU_BAUD_REG,270);             //Set baud rate to 115200

    put32(AUX_MU_CNTL_REG,3);               //Finally, enable transmitter and receiver
}

// return 0 for match, 1 for not.
int uart_probe(struct dtn *node)
{
    if (strstr(node->compatible, "uart")) {
        uart_init();
        return 0;
    }
    return 1;
}

void uart_send ( char c )
{
    while(1) {
        if(get32(AUX_MU_LSR_REG)&0x20)      // 5th bit is set if the transmit FIFO can accept at least one byte.
            break;
    }
    put32(AUX_MU_IO_REG,c);
}

#define UART_BUF_SIZE 128
extern char uart_send_buf[];
extern int uart_send_buf_in;
extern int uart_send_buf_out;
void uart_send_async ( char c )
{
    while ((uart_send_buf_in + 1) % UART_BUF_SIZE == uart_send_buf_out)
        ;
    uart_send_buf[uart_send_buf_in] = c;
    uart_send_buf_in = (uart_send_buf_in + 1) % UART_BUF_SIZE;

    put32(AUX_MU_IER_REG, 3); //Enable receive and transmit interrupts

}

char uart_recv ( void )
{
    while(1) {
        if(get32(AUX_MU_LSR_REG)&0x01)      // 0th bit is set if the receive FIFO holds at least 1 symbol.
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

void uart_send_string_async(char* str)
{
    for (int i = 0; str[i] != '\0'; i ++) {
        uart_send_async((char)str[i]);
    }
}
// extern char uart_send_buf[];

// void uart_send_string_low_power(char* str)
// {

//     for (int i = 0; str[i] != '\0'; i ++) {
//         uart_send_buf[i] = str[i];
//     }

//     while (!recv_ready) {
//         wait_for_interrupt(); // low power standby
//         // uart_send_string("wfi\r\n");
//     }
// }

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

extern char uart_recv_buf[];
extern int recv_ready;
int read_line_low_power(char buf[], int buf_size)
{
    char *buf_ptr, *cmd_buf_ptr;
    int cnt;

    while (!recv_ready) {
        wait_for_interrupt(); // low power standby
        // uart_send_string("wfi\r\n");
    }

    buf_ptr = buf;
    cmd_buf_ptr = uart_recv_buf;
    cnt = 0;
    while ((*cmd_buf_ptr != '\0') && (++cnt < buf_size))
        *buf_ptr++ = *cmd_buf_ptr++;
    *buf_ptr = '\0';
    uart_send_string("\r\n");
    // Clear recv_ready, so uart interrupt can
    // start to fill content into cmd_buf
    recv_ready = 0;
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
    int i = 0, j, neg_flag = 0;
    char str[100];
    
    if (number < 0) {
        neg_flag = 1;
        number = (-1) * number;
    }
    while (number) {
        str[i++] = (number % 10) + '0';
        number = number / 10;
    }

    reverse(str, i);
    if (neg_flag)
        uart_send('-');
    for ( j = 0 ; j < i ; j++) {
        uart_send(str[j]);
    }
    // uart_send('\r\n');
    return ; 
}

void uart_send_uint(unsigned int number) {
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

void uart_send_ulong(unsigned long number) {
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


// #define CMD_SIZE 64
// #define UART_BUF_SIZE 128
// char uart_send_buf[UART_BUF_SIZE];
// int uart_send_buf_idx = 0;
// char uart_recv_buf[UART_BUF_SIZE];
// int uart_recv_buf_idx = 0;
// char cmd_buf[CMD_SIZE];

// /* AUX_MU_IIR_REG: peripheral p.13 */
// void do_uart_handler()
// {
//     char ch;
//     unsigned int iir;
//     int cnt;

//     iir = get32(AUX_MU_IIR_REG);

//     if ((iir & 0x6) == 0x2)
//     { // Transmit holding register empty 
//         // while(get32(AUX_MU_LSR_REG));
//         ;
//     }
//     else if ((iir & 0x6) == 0x4)
//     { // Receiver holds valid byte
//         // while (get32(AUX_MU_LSR_REG) & 0x1) {
//         if (get32(AUX_MU_LSR_REG) & 0x1) {
//             // The receive FIFO holds at least 1 symbol.
//             ch = get32(AUX_MU_IO_REG) & 0xFF;
//             uart_recv_buf[uart_recv_buf_idx++] = ch;
//             // uart_recv_buf_idx = (uart_recv_buf_idx + 1) % UART_BUF_SIZE;
//             uart_send_buf[uart_send_buf_idx++] = ch;
//             while(1) {
//                 if(get32(AUX_MU_LSR_REG)&0x20)      // 5th bit is set if the transmit FIFO can accept at least one byte.
//                     break;
//             }
//             put32(AUX_MU_IO_REG,c);

//         }
//     }
// }