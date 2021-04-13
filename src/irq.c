#include "irq.h"

void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1){
    return;
}
void handle_el1_irq(){
    unsigned int irq_sig2 = get32(IRQ_PENDING_1);
    unsigned int irq_sig1 = get32(CORE0_INTERRUPT_SOURCE);
    if(irq_sig2 & AUX_IRQ){
        // uart_puts("uart el1\r\n");
        uart_irq();
        enable_irq();
    }
    if(irq_sig1 == 2){ // CNTPNIRQ
        // uart_puts("core timer el1\r\n");
        disable_irq();
        el1_timer_irq();
        enable_irq();
    }
    return;
}
void handle_el0_irq(){
    unsigned int irq_sig2 = get32(IRQ_PENDING_1);
    unsigned int irq_sig1 = get32(CORE0_INTERRUPT_SOURCE);
    if(irq_sig2 & AUX_IRQ){
        // uart_puts("uart\r\n");
        uart_irq();
        enable_irq();
    }
    if(irq_sig1 == 2){
        // uart_puts("core time\r\n");
        el0_timer_irq();
        enable_irq();
    }
    // else disable_irq();
    return;
}

