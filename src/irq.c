#include "irq.h"

void handle_sync_el1(unsigned long esr_el1, unsigned long elr_el1){
    unsigned int* addr = elr_el1 - 4;
    unsigned int imm_value = (*addr & 0x000fffff) >> 5; 
    
    if(imm_value & 0x03){
        uart_puts("gg123\r\n");
        // enable_uart_interrupt();
        // enable_irq();
    }
    return;
}
void handle_el1_irq(){
    unsigned int irq_sig2 = get32(IRQ_PENDING_1);
    unsigned int irq_sig1 = get32(CORE0_INTERRUPT_SOURCE);
    if(irq_sig2 & AUX_IRQ){
        // uart_puts("???\r\n");
        uart_irq();
        enable_irq();
    }
    if(irq_sig1 == 2){ // CNTPNIRQ
        // uart_puts("core time123\r\n");
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
        //uart_puts("core time\r\n");
        el0_timer_irq();
        enable_irq();
    }
    // else disable_irq();
    return;
}

