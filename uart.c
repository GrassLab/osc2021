#include "aux.h"
#include "gpio.h"

void uart_init() {
    *AUX_ENABLES |= 1;  // bitwise or for not modifying the other 31 bits
    *AUX_MU_IER = 0;    // disable interrupt during uart configuration
    *AUX_MU_IIR = 0;    // 
    *AUX_MU_CNTL = 0;   // disable Rx/Tx during uart configuration
    *AUX_MU_LCR = 3;    // set bandwidth to 8bits

}