#include "auxiliary.h"

void uart_init() {
    // auxiliary setting
    *AUX_ENABLES = 1;
    *AUX_MU_CNTL_REG = 0;
    *AUX_MU_IER_REG = 0;
    *AUX_MU_LCR_REG = 3;
    *AUX_MU_MCR_REG = 0;
    *AUX_MU_BAUD_REG = 270;
    *AUX_MU_IIR_REG = 6;

    // GPIO setting
    unsigned int r = *GPFSEL1;
    // reset gpio 14 15
    r &= ~((0b111<<12)|(0b111<<15));
    // set alt5 to gpio 14 15
    r |= (0b010<<12)|(0b010<<15);
    *GPFSEL1 = r;

    // disable gpio pull-down pull-up
    *GPPUD = 0;
    // wait 150 cycles
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = (0b1<<14)|(0b1<<15);
    r = 150;
    while(r--) {
        asm volatile("nop");
    }
    *GPPUDCLK0 = 0;

    // enable Rx Tx
    *AUX_MU_CNTL_REG = 3;
}
