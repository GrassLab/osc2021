#include "gpio.h"
#include "uart.h"

#define AUX_MU_IO_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR_REG ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_MSR_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215058))
#define AUX_MU_SCRATCH ((volatile unsigned int*)(MMIO_BASE + 0x0021505C))
#define AUX_MU_CNTL_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_STAT_REG ((volatile unsigned int*)(MMIO_BASE + 0x00215064))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))
#define AUXENB ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
int uart_interrupt_flag, cmd_flag;
int uart_buffer_idx, read_buffer_idx, cmd_idx;
char UART_BUFFER[UART_BUFFER_SIZE], CMD_BUFFER[UART_BUFFER_SIZE];

void uart_init(){
    register unsigned int reg;
    *AUXENB |= 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0x00; // disable interrupt
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR_REG = 0xc6;
    reg = *GPFSEL1;
    reg &= ~((7 << 12) | (7 << 15));
    reg |= (2 << 12) | (2 << 15);
    *GPFSEL1 = reg;
    *GPPUD = 0;
    reg = 150;
    while(reg--) { asm volatile("nop");}
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    reg = 150;
    while(reg--) { asm volatile("nop");}
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL_REG = 3;
    cmd_flag = uart_interrupt_flag = 0;
    cmd_idx = read_buffer_idx = uart_buffer_idx = 0;
    
}
void enable_uart_interrupt(){
    put32(ENB_IRQS1, AUX_IRQ);
    return;
}
void uart_send_string(char* str){
    for(int i = 0; str[i] != '\0'; ++i){
        UART_BUFFER[uart_buffer_idx++] = str[i];
        if(uart_buffer_idx == UART_BUFFER_SIZE)
            uart_buffer_idx = 0;
    }
    if(uart_interrupt_flag == 0){
        *AUX_MU_IER_REG = 0x03; // enable interrupt
        uart_interrupt_flag = 1;
    }
}
void uart_irq(){
    unsigned int id = *AUX_MU_IIR_REG;
    //uart_printhex(id);
    char c;
    disable_irq();
    if((id & 0x06) == 0x04){ // write
        //uart_puts("456\r\n");
        if(uart_interrupt_flag == 0){
            *AUX_MU_IER_REG = 0x03; // enable interrupt
            uart_interrupt_flag = 1;
        }
        
        while((*AUX_MU_LSR_REG)&0x01){
            c = (char)(*AUX_MU_IO_REG);
            if(c == '\r'){
                // cmd
                CMD_BUFFER[cmd_idx++] = '\0';
                cmd_flag = 1;
                cmd_idx = 0;
                // uart
                UART_BUFFER[uart_buffer_idx++] = c;
                if(uart_buffer_idx == UART_BUFFER_SIZE) uart_buffer_idx = 0;
                UART_BUFFER[uart_buffer_idx++] = '\n';
            }
            else{
                UART_BUFFER[uart_buffer_idx++] = c;
                CMD_BUFFER[cmd_idx++] = c;
            }
            if(uart_buffer_idx == UART_BUFFER_SIZE) uart_buffer_idx = 0;
        }
    }
    if((id & 0x06) == 0x02){ 
        while((*AUX_MU_LSR_REG)&0x20){
            if(read_buffer_idx == uart_buffer_idx){
                //uart_puts("123\r\n");
                *AUX_MU_IER_REG = 0x01; // disable interrupt
                uart_interrupt_flag = 0;
                return;
            }
            c = UART_BUFFER[read_buffer_idx++];
            *AUX_MU_IO_REG = c;
            if(read_buffer_idx == UART_BUFFER_SIZE)
                read_buffer_idx = 0;
        }
        //enable_irq();
    }
    return;
}

void uart_send(unsigned int c){
    do {
    	asm volatile("nop");
    }while(~(*AUX_MU_LSR_REG)&0x20);
    *AUX_MU_IO_REG = c;
	
}
void uart_puts(char *s){
    while(*s){
    	if(*s == '\n') uart_send('\r');
	// if(*s == '\r') uart_send('\n');
	uart_send(*s++);
    }
}
void uart_puts_bySize(char *s, int size){
    for(int i = 0; i < size ;++i){
        if(*s == '\n') uart_send('\r');
        uart_send(*s++);
    }
}

void uart_printint(unsigned long long int num){
    if(num == 0) uart_send('0');
    else{
        if(num >= 10) uart_printint(num / 10);
        uart_send(num % 10 + '0');
    }
    //uart_puts("\r\n");
}
void uart_printhex(unsigned long long int num){
    char res[16];
    for(int i = 0; i < 8; ++i, num >>= 4){
        short int tmp = num % 16;
        if(tmp >= 10) res[i] = tmp - 10 + 'a';
        else res[i] = tmp + '0';
    }
    uart_puts("0x");
    for(int i = 7; i >= 0; --i) uart_send(res[i]);
    //uart_puts("\r\n");
}
char uart_get(){
    char res;
    do {
    	asm volatile("nop");
    } while(~(*AUX_MU_LSR_REG)&0x01);
    res = (char)(*AUX_MU_IO_REG);
    // if(res == '\r') return '\n';
    return res;
}


int uart_get_int(){
    int res = 0;
    char c;
    while(1){
        c = uart_get();
        if(c == '\0' || c == '\n')
            break;
        uart_send(c);
        res = res * 10 + (c - '0');
    }
    return res;
}