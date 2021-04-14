#include "interrupt_handler.h"

extern uint64_t get_currentEL();

void sys_call(uint64_t spsr_el1, uint64_t elr_el1, uint64_t esr_el1){
    miniuart_send_S("This is a sys_call:" NEW_LINE);
    printf("spsr_el1 = %x, elr_el1 = %x, esr_el1 = %x" NEW_LINE, spsr_el1, elr_el1, esr_el1);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
}

void print_core_timer(uint64_t cntfrq_el0, uint64_t cntpct_el0){
    printf("core time = %d seconds" NEW_LINE, cntpct_el0/cntfrq_el0);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
}

void dump_interrupt_reg(uint64_t type, uint64_t elr_el1, uint64_t esr_el1){
    printf("type = %d, elr_el1 = %x, esr_el1 = %x" NEW_LINE, type, elr_el1, esr_el1);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
    volatile uint32_t *p = (uint32_t *)0x3F00B200U;
    printf("IRQ basic pending = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B204U;
    printf("IRQ pending 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B208U;
    printf("IRQ pending 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B20CU;
    printf("FIQ control = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B210U;
    printf("Enable IRQs 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B214U;
    printf("Enable IRQs 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B218U;
    printf("Enable Basic IRQs = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B21CU;
    printf("Disable IRQs 1 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B220U;
    printf("Disable IRQs 2 = %x" NEW_LINE, *p);
    p = (uint32_t *)0x3F00B224U;
    printf("Disable Basic IRQs = %x" NEW_LINE, *p);
    miniuart_send_nC('-', 22);
    miniuart_send_S(NEW_LINE);
}
