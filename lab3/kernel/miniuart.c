#include "miniuart.h"

static uint64_t BCM2835_GPIO_ADDR;
static uint64_t BCM2835_AUX_ADDR;
static uint64_t BCM2835_AUX_UART_ADDR;

extern void delay(uint32_t cycle);
extern void Infinite_Loop(uint32_t error_code);

void miniuart_init(){
    char buffer[128];
    uint32_t value[16];

    const uint32_t *prop = dtb_get_node_prop_addr("aliases", NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    int32_t n = dtb_get_prop_value("gpio", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    char *node_name = buffer;
    while(1){
        char *token = strtok(node_name, '/');
        if(token == NULL){
            break;
        }
        node_name = token;
    }

    prop = dtb_get_node_prop_addr(node_name, NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    n = dtb_get_prop_value("compatible", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(strcmp(buffer, "brcm,bcm2835-gpio")){
        Infinite_Loop(3);
    }

    n = dtb_get_prop_value("reg", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(n != 8){
        Infinite_Loop(4);
    }
    uint32_t base = bswap32(value[0]);

    n = dtb_get_parent_prop_value("ranges", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    for(int32_t i=0; i<n/3; i+=3){
        if(base >= bswap32(value[i])){
            if(base <= bswap32(value[i]) + bswap32(value[i+2])){
                base = base - bswap32(value[i]) + bswap32(value[i+1]);
                break;
            }
        }
    }
    BCM2835_GPIO_ADDR = base;

    prop = dtb_get_node_prop_addr("aliases", NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    n = dtb_get_prop_value("aux", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    node_name = buffer;
    while(1){
        char *token = strtok(node_name, '/');
        if(token == NULL){
            break;
        }
        node_name = token;
    }

    prop = dtb_get_node_prop_addr(node_name, NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    n = dtb_get_prop_value("compatible", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(strcmp(buffer, "brcm,bcm2835-aux")){
        Infinite_Loop(3);
    }

    n = dtb_get_prop_value("reg", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(n != 8){
        Infinite_Loop(4);
    }
    base = bswap32(value[0]);

    n = dtb_get_parent_prop_value("ranges", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    for(int32_t i=0; i<n/3; i+=3){
        if(base >= bswap32(value[i])){
            if(base <= bswap32(value[i]) + bswap32(value[i+2])){
                base = base - bswap32(value[i]) + bswap32(value[i+1]);
                break;
            }
        }
    }
    BCM2835_AUX_ADDR = base;

    prop = dtb_get_node_prop_addr("aliases", NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    n = dtb_get_prop_value("uart1", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    node_name = buffer;
    while(1){
        char *token = strtok(node_name, '/');
        if(token == NULL){
            break;
        }
        node_name = token;
    }

    prop = dtb_get_node_prop_addr(node_name, NULL);
    if(!prop){
        Infinite_Loop(1);
    }
    n = dtb_get_prop_value("compatible", prop, buffer);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(strcmp(buffer, "brcm,bcm2835-aux-uart")){
        Infinite_Loop(3);
    }

    n = dtb_get_prop_value("reg", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    if(n != 8){
        Infinite_Loop(4);
    }
    base = bswap32(value[0]);

    n = dtb_get_parent_prop_value("ranges", prop, value);
    if(n == -1){
        Infinite_Loop(2);
    }
    for(int32_t i=0; i<n/3; i+=3){
        if(base >= bswap32(value[i])){
            if(base <= bswap32(value[i]) + bswap32(value[i+2])){
                base = base - bswap32(value[i]) + bswap32(value[i+1]);
                break;
            }
        }
    }
    BCM2835_AUX_UART_ADDR = base;

    SET_REG(*GPFSEL1, 0x0003F000, 0x00012000);  //GPIO 14, 15 -> alt5
    *GPPUD &= ~(0b11);  //disable pull-up/down
    delay(150);
    *GPPUDCLK0 |= (1 << 14) | (1 << 15);
    delay(150);
    *GPPUDCLK0 &= ~((1 << 14) | (1 << 15));

    *AUX_ENABLES |= 1;  //enable Mini UART
    *AUX_MU_CNTL_REG &= ~(0b11);  //disable TX RX
    *AUX_MU_IER_REG &= ~(0b11);  //disable interrupt
    *AUX_MU_LCR_REG |= 0b11;  //set 8 bit data size
    *AUX_MU_MCR_REG &= ~(0b10);  //Set RTS line to high
    SET_REG(*AUX_MU_BAUD_REG, 0x0000FFFF, 270);  //set baud rate to 115200
    *AUX_MU_IIR_REG |= 0b110;  //clear FIFO
    *AUX_MU_CNTL_REG |= 0b11;  //enable TX RX
}

char miniuart_recv(){
    while(!(*AUX_MU_LSR_REG & 1));  //Data ready
    return *AUX_MU_IO_REG & 0xFF;
}

uint32_t miniuart_send_C(char c){
    while(!(*AUX_MU_LSR_REG & 0x20));  //Transmitter empty
    *AUX_MU_IO_REG = c;
    return 1;
}

uint32_t miniuart_send_nC(char c, size_t n){
    while(n--){
        miniuart_send_C(c);
    }
    return n;
}

uint32_t miniuart_send_S(const char *str){
    uint32_t n = 0;
    while(*str){
        miniuart_send_C(*str++);
        n++;
    }
    return n;
}

uint32_t miniuart_send_nS(const char *str, size_t n){
    while(*str && n--){
        miniuart_send_C(*str++);
    }
    return n;
}
