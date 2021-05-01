#include "irq.h"
#include "aux.h"
#include "uart.h"

static void uart_interrupt_handler() {
    if ((*AUX_MU_IIR & 0x4)) {
        char c = *AUX_MU_IO;
        uart_putc(c);
        if (c == '\r') {
            uart_putc('\n');
            end_of_input = true;
        } else {
            buffer_push(c, uart_in);
        }
    } else if ((*AUX_MU_IIR & 0x2)) {
        char c;
        if ((c = buffer_pop(uart_out)) == (char)0) {
            *AUX_MU_IER &= 0x1;
        } else {
            *AUX_MU_IO = c;
        }
    }
}

static void timer_interrupt_handler() {
    asm volatile("stp x29, x30, [sp,#-32]!");
    asm volatile("mov x29, sp");
    asm volatile("mrs    x0, cntpct_el0");
    asm volatile("mrs    x1, cntfrq_el0");
    asm volatile("bl     print_timer");
    asm volatile("mrs    x1, cntfrq_el0");
    asm volatile("add    x0, xzr, x1, lsl #1");
    asm volatile("msr    cntp_tval_el0, x0");
    asm volatile("ldp x29, x30, [sp], #32");
}

void irq_route() {
    if ((*CORE0_IRQ_SRC & 0x2)) {
        timer_interrupt_handler();
    } else if ((*GPU_PEND1 & (1 << 29))) {
        uart_interrupt_handler();
    }
}
