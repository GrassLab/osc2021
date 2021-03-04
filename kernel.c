#define GETS_BUFF_LEN 0xff

#define MMIO_BASE 0x3F000000

#define UART_MAP_OFFSET 0x00215000
#define GPIO_MAP_OFFSET 0x00200000

#define AUX_IRQ            ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x00))
#define AUX_ENABLES        ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x04))
#define AUX_MU_IO_REG      ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x40))
#define AUX_MU_IER_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x44))
#define AUX_MU_IIR_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x48))
#define AUX_MU_LCR_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x4C))
#define AUX_MU_MCR_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x50))
#define AUX_MU_LSR_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x54))
#define AUX_MU_MSR_REG     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x58))
#define AUX_MU_SCRATCH     ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x5C))
#define AUX_MU_CNTL_REG    ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x60))
#define AUX_MU_STAT_REG    ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x64))
#define AUX_MU_BAUD_REG    ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x68))
#define AUX_SPI0_CNTL0_REG ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x80))
#define AUX_SPI0_CNTL1_REG ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x84))
#define AUX_SPI0_STAT_REG  ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x88))
#define AUX_SPI0_IO_REG    ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x90))
#define AUX_SPI0_PEEK_REG  ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0x94))
#define AUX_SPI1_CNTL0_REG ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0xC0))
#define AUX_SPI1_CNTL1_REG ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0xC4))
#define AUX_SPI1_STAT_REG  ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0xC8))
#define AUX_SPI1_IO_REG    ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0xD0))
#define AUX_SPI1_PEEK_REG  ((volatile unsigned int*)(MMIO_BASE + UART_MAP_OFFSET + 0xD4))

#define GPFSEL0            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x00))
#define GPFSEL1            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x04))
#define GPFSEL2            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x08))
#define GPFSEL3            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x0C))
#define GPFSEL4            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x10))
#define GPFSEL5            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x14))
#define GPSET0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x1C))
#define GPSET1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x20))
#define GPCRL0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x28))
#define GPCRL1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x2C))
#define GPLEV0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x34))
#define GPLEV1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x38))
#define GPEDS0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x40))
#define GPEDS1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x44))
#define GPREN0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x4C))
#define GPREN1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x50))
#define GPFEN0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x58))
#define GPFEN1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x5C))
#define GPHEN0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x64))
#define GPHEN1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x68))
#define GPLEN0             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x70))
#define GPLEN1             ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x74))
#define GPAREN0            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x7C))
#define GPAREN1            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x80))
#define GPAFEN0            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x88))
#define GPAFEN1            ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x8C))
#define GPPUD              ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x94))
#define GPPUDCLK0          ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x98))
#define GPPUDCLK1          ((volatile unsigned int*)(MMIO_BASE + GPIO_MAP_OFFSET + 0x9C))

void uart_init() {
    int cycle=0;
    // GPIO
    *GPFSEL1 &= ~(0b111<<12); // clear GPIO pin 14 alnt function
    *GPFSEL1 |= (0b010<<12);  // GPIO Pin 14 takes alternate function 5
    *GPFSEL1 &= ~(0b111<<15); // clear GPIO pin 15 alt function
    *GPFSEL1 |= (0b010<<15);  // GPIO Pin 15 takes alternate function 5
    *GPPUD = 0b00; // Disable GPIO pull up/down.
    cycle=150; while(cycle--) { asm volatile("nop"); } // busy waiting for 150 cycle
    *GPPUDCLK0 = (0b11<<14); // trigger ping 14, 15
    cycle=150; while(cycle--) { asm volatile("nop"); }
    *GPPUD = 0;
    *GPPUDCLK0 = 0;
    // UART
    *AUX_ENABLES |= 1;       // enable mini UART.
    *AUX_MU_CNTL_REG = 0;   // Disable transmitter and receiver during configuration.
    *AUX_MU_IER_REG = 0;    // Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_LCR_REG = 3;    // Set the data size to 8 bit.
    *AUX_MU_MCR_REG = 0;    // Don’t need auto flow control.
    *AUX_MU_BAUD_REG = 270; // Set baud rate to 115200
    *AUX_MU_IIR_REG = 6;    // No FIFO.
    *AUX_MU_CNTL_REG = 3;   // Enable the transmitter and receiver.
    return;
}

char uart_getc() {
    char c;
    while(!(*AUX_MU_LSR_REG & 0b1)) { asm volatile("nop"); }
    c = (char)*AUX_MU_IO_REG;
    //asm volatile("nop");
    return c=='\r'?'\n':c;
}

void uart_setc(char c) {
    while(!(*AUX_MU_LSR_REG & 0b1<<5)) { asm volatile("nop"); }
    *AUX_MU_IO_REG = c;
    return;
}

char * gets (char * str) {
    char c = '\0';
    //char buff[GETS_BUFF_LEN];
    int buff_end = 0;
    do {
        c = uart_getc();
        str[buff_end++] = c;
        uart_setc(c);
    } while (c != '\n');
    str[buff_end-1] = '\0';
    return str;
}

int puts(const char * str) {
    int str_end = 0;
    while (str[str_end] != '\0') {
        uart_setc(str[str_end++]);
    }
    return str_end;
}

void help() {
    puts("Commands:\n");
    puts("    help\n");
    puts("    hello\n");
    puts("    reboot\n");
    return;
}

void hello() {
    puts("Hello World!\n");
    return;
}

// 0 as same, 1 as different, -1 as error
int strcmp(const char * a, const char * b) {
    int cnt = 0;
    while (a[cnt] != '\0' && b[cnt] != '\0') {
        if (a[cnt] != b[cnt]) {
            return 1;
        }
        if (cnt >= 1000) {
            return -1;
        }
        cnt++;
    }
    if (a[cnt] == '\0' && b[cnt] == '\0') {
        return 0;
    }
    return 1;
}

#define PM_PASSWORD 0x5a000000
#define PM_RSTC 0x3F10001c
#define PM_WDOG 0x3F100024

void reset(int tick){ // reboot after watchdog timer expire
    *((unsigned int*)PM_RSTC) = PM_PASSWORD | 0x20; // full reset
    *((unsigned int*)PM_WDOG) = PM_PASSWORD | tick; // number of watchdog tick
}

//void cancel_reset() {
//    set(PM_RSTC, PM_PASSWORD | 0); // full reset
//    set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick
//}

void reboot() {
    reset(1);
}

void cmd(const char * buff) {
    if (!strcmp(buff, "help")) {
        help();
    } else if (!strcmp(buff, "hello")) {
        hello();
    } else if (!strcmp(buff, "reboot")) {
        reboot();
    } else if (!strcmp(buff, "")) {
        return;
    }else {
        puts(buff);
        puts(" : command not found!\n");
    }
    return;
}

void kernel() {
    uart_init();
    uart_setc('H');
    uart_setc('i');
    uart_setc('\n');
    while(1) {
        char buff[GETS_BUFF_LEN];
        gets(buff);
        cmd(buff);
    }
    return;
}
