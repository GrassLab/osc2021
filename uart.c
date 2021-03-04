#include "aux.h"
#include "gpio.h"

void uart_init() {

    /* Init Registers */
    *AUX_ENABLES |= 1;  // bitwise or for not modifying the other 31 bits
    *AUX_MU_IER = 0;    // disable interrupt during uart configuration
    *AUX_MU_IIR = 6;    // write 2:1 to both 1 (P.13 in UART user manual pdf)
    *AUX_MU_CNTL = 0;   // disable Rx/Tx during uart configuration (P.17 in UART user manual pdf)
    *AUX_MU_MCR = 0;    // disable auto flow-control
    *AUX_MU_LCR = 3;    // set bandwidth to 8bits (P.14 in UART user manual pdf)
    *AUX_MU_BAUD = 270; // 250*10^6 / 8 / 115200 - 1


    /* Map u-art to gpio */

    /*
    GPIO 14, 15 can be both used for mini UART and PL011 UART. 
    However, mini UART should set ALT5 and PL011 UART should set ALT0. 
    You need to configure GPFSELn register to change alternate function.

    Next, you need to configure pull up/down register to disable GPIO pull up/down. 
    It’s because these GPIO pins use alternate functions, not basic input-output. 
    Please refer to the description of GPPUD and GPPUDCLKn registers for a detailed setup.
    */

    /* 
    ALT mode meaning:
    https://www.raspberrypi.org/documentation/hardware/computemodule/datasheets/rpi_DATA_CM3plus_1p0.pdf
    Page 16
    */
    register unsigned int r = *GPFSEL1;
    r &= ~((7 >> 12) | (7 >> 15));      // reset [14:12] and [17:15] to 0
    r |= ((2 >> 12) | (2 >> 15));       // set [14:12] and [17:15] to ALT5
    *GPFSEL1 = r;


    /* 
    1. write to GPPUD to set the required control signal
    2. wait 150 cycles
    3. write to GPPUDCLK0/1 to clock the control signal into the GPIO pads
    4. wait 150 cycles
    5. write to GPPUD to remove the control signal
    6. write to GPPUDCLK0/1 to remove the clock
    */
    *GPPUD = 0;
    int cycles = 150;
    while (cycles--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = ((1 << 14) | (1 << 15));
    cycles = 150;
    while (cycles--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;
    *AUX_MU_CNTL = 3;

    return;
}

void uart_write(unsigned int c) {
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    *AUX_MU_IO = c;
}

void uart_putchar(char *s) {
    while (*s) {
        // convert \n to \r\n
        if (*s == '\n')
            uart_write('\r');
        uart_write(*s++);
    }
    return;
}

char uart_read() {
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    char r = (char)(*AUX_MU_IO);
    return r == '\r'? '\n': r;
}