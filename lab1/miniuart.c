#include "bcm2837.h"

extern void delay(uint32_t cycle);

void miniuart_init(){
    SET_REG(*GPFSEL1, 0x0003F000, 0x00012000);  //GPIO 14, 15 -> alt5
    *GPPUD &= !(0b11);  //disable pull-up/down
    delay(150);
    *GPPUDCLK0 |= (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 &= !((1 << 14) | (1 << 15));

    *AUX_ENABLES |= 1;  //enable Mini UART
    *AUX_MU_CNTL_REG &= !(0b11);  //disable TX RX
    *AUX_MU_IER_REG &= !(0b11);  //disable interrupt
    *AUX_MU_LCR_REG |= 0b11;  //set 8 bit data size
    *AUX_MU_MCR_REG &= !(0b10);  //Set RTS line to high
    SET_REG(*AUX_MU_BAUD_REG, 0x0000FFFF, 270);  //set baud rate to 115200
    *AUX_MU_IIR_REG |= 0b110;  //clear FIFO
    *AUX_MU_CNTL_REG |= 0b11;  //enable TX RX
}

char miniuart_recv(){
    while(!(*AUX_MU_LSR_REG & 1));  //Data ready
    return *AUX_MU_IO_REG & 0xFF;
}

void miniuart_send_C(char c){
    while(!(*AUX_MU_LSR_REG & 0x20));  //Transmitter empty
    *AUX_MU_IO_REG = c;
}

void miniuart_send_S(const char *str){
    while(*str){
        miniuart_send_C(*str++);
    }
}
