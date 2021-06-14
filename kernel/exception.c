#include "uart.h"
#include "scheduler.h"

void dumpReg() {
    uart_puts("dump sctlr...\n");
    unsigned long sctlr_el1;
    asm volatile("mrs %0, sctlr_el2 \n":"=r"(sctlr_el1):);
    uart_puts_int(sctlr_el1);
    uart_puts("\n");
}

void dumpState() {
    unsigned long spsr_el1, elr_el1, esr_el1, sctlr_el1, far_el1;
    asm volatile("mrs %0, spsr_el1 \n":"=r"(spsr_el1):);
    asm volatile("mrs %0, elr_el1 \n":"=r"(elr_el1):);
    asm volatile("mrs %0, esr_el1 \n":"=r"(esr_el1):);
    asm volatile("mrs %0, sctlr_el1 \n":"=r"(sctlr_el1):);
    asm volatile("mrs %0, far_el1 \n":"=r"(far_el1):);

    uart_puts("-----dump state-----\n");
    uart_puts("SPSR_EL1: 0x");
    uart_puts_hex(spsr_el1);
    uart_puts("\n");
    uart_puts("ELR_EL1: 0x");
    uart_puts_hex(elr_el1);
    uart_puts("\n");
    uart_puts("ESR_EL1: 0x");
    uart_puts_hex(esr_el1);
    uart_puts("\n");
    uart_puts("SCTLR_EL1:0x");
    uart_puts_hex(sctlr_el1);
    uart_puts("\n");
    uart_printf("FAR_EL1: 0x%d\n", far_el1);
    uart_puts("---------------------\n");
    while(1) {}
}

void dumpTimer() {
    asm volatile("mrs x0, cntfrq_el0 \n");
    asm volatile("add x0, x0, x0 \n"); // 2 second
    asm volatile("msr cntp_tval_el0, x0 \n");
    timer_tick();
    
    unsigned long cntpct, cntfrq, tmp;
    asm volatile("mrs %0, cntpct_el0 \n":"=r"(cntpct):);
    asm volatile("mrs %0, cntfrq_el0 \n":"=r"(cntfrq):);
    tmp = cntpct/cntfrq;
    uart_puts("-----Timer-----\n");
    uart_puts("Time Elapsed: ");
    uart_puts_int(tmp);
    uart_puts("\n");
    uart_puts("---------------\n");
    
}