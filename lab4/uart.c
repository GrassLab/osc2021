#include "gpio.h"
#include "uart.h"
#include "dtb.h"
#include "string.h"
#include "util.h"

#define AUX_ENABLE  ((volatile unsigned int*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO   ((volatile unsigned int*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER  ((volatile unsigned int*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR  ((volatile unsigned int*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR  ((volatile unsigned int*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR  ((volatile unsigned int*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR  ((volatile unsigned int*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL ((volatile unsigned int*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD ((volatile unsigned int*)(MMIO_BASE + 0x00215068))

#define BUFFER_SIZE         50

void uart_init() {
    *AUX_ENABLE |= 1;
    *AUX_MU_CNTL = 0;
    *AUX_MU_IER = 0;
    *AUX_MU_LCR = 3;
    *AUX_MU_MCR = 0;
    *AUX_MU_BAUD = 270;
    *AUX_MU_IIR = 6;
    *GPFSEL1 &= ~((7 << 12) | (7 << 15));
    *GPFSEL1 |= (2 << 12) | (2 << 15);
    
    *GPPUD = 0;
    for (int i = 0; i < 150; i++) asm volatile("nop");
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    for (int i = 0; i < 150; i++) asm volatile("nop");
    *GPPUDCLK0 = 0;
    
    *AUX_MU_CNTL = 3;
}

char uart_get_char() {
    char c;
    while ( !(*AUX_MU_LSR&0x01) );
    c = (char)(*AUX_MU_IO);
    return c;
}

char *uart_get_str() {
    static char str[BUFFER_SIZE];
    int counter = 0;

    for (int i = 0 ; i < BUFFER_SIZE ; i++) {
        str[i] = '\0';
    }

    while (1) {
        str[counter] = uart_get_char();
        if (str[counter] == '\r')
            str[counter] = '\n';
        uart_send(str[counter]);
        if (str[counter] == '\n') {
            str[counter] = '\0';
            return str;
        }
        if (counter >= BUFFER_SIZE - 1) {
            str[BUFFER_SIZE - 1] = '\0';
            uart_put_str("\nBuffer is full.\n");
            return str;
        }
        counter++;
    }
}

int uart_get_int() {
    unsigned int num;
    num = uart_get_char()<<24;
    num |= uart_get_char()<<16;
    num |= uart_get_char()<<8;
    num |= uart_get_char();
    return num;
}

void uart_send(unsigned int c) {
    if (c == '\n')
        uart_send('\r');
    
    while ( !(*AUX_MU_LSR&0x20) );
    *AUX_MU_IO = c;
}

void uart_put_str(char *s) {
    while (*s)
        uart_send(*s++);
}

void uart_put_int(int num) {
    if (num/10)
		uart_put_int(num/10);
    uart_send((num%10) + '0');
}

void uart_put_ulong(unsigned long num) {
    if (num/10)
		uart_put_int(num/10);
    uart_send((num%10) + '0');
}

void uart_put_addr(unsigned long addr) {
    uart_put_str("0x");
    for (int i = 15;i >= 0; i--) {
        int num = (addr >> (i * 4)) & 0xf;
        num += '0';
        if (num > '9')
            num += 'a' - ('9' + 1);
        uart_send(num);
    }
}

int uart_probe(fdt_header *header, unsigned long node_addr, int depth) {
    unsigned long str_block_addr = (unsigned long)header + big_to_little_32(header->off_dt_strings);
    unsigned int *token_addr = (unsigned int *)node_addr;

    if (strstr(((ftd_node_header *)node_addr)->name, "serial", 6) && depth == 2) {
        token_addr = find_next_token(token_addr);
        do {
            switch (big_to_little_32(*token_addr)) {
                case FDT_PROP: {
                    fdt_node_prop *prop = (fdt_node_prop *)token_addr;
                    
                    if (strcmp((char *)(str_block_addr + big_to_little_32(prop->nameoff)), "compatible")) {
                        
                        int len = big_to_little_32(prop->len);
                        char *str = (char *)(prop + 1);
                        for (int i = 0; i < len;) {
                            if (strstr(&str[i], "uart", 4)) {
                                uart_init();
                                uart_put_str("uart_init\n");
                                return 0;
                            }
                            while (str[i++]);
                        }
                    }
                    break;
                }

                case FDT_BEGIN_NODE:
                case FDT_END:
                    return 0;
                
                default:
                    break;
            }
        } while((token_addr = find_next_token(token_addr)));
    }
    return 0;
}