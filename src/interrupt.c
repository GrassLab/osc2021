#include "irq.h"
#include "aux.h"
#include "uart.h"

/* Time slice = 1(sec) / 2 ^ 3 */
extern bool_t resched_flag;
extern size_t read_size;
extern int remaining_time;

int wake_up(uint16_t);

static void uart_interrupt_handler() {
    if ((*AUX_MU_IIR & 0x4)) {
        if (buffer_full(uart_in) == false) {
            char c = *AUX_MU_IO;
            async_uart_putc(c);
            buffer_push(c, uart_in);
            read_size--;
            if (read_size == 0) {
                if (wake_up(0) < 0)
                    *AUX_MU_IER &= 0x2;
            }
        }
    } else if ((*AUX_MU_IIR & 0x2)) {
        if (buffer_empty(uart_out) == true) {
            *AUX_MU_IER &= 0x1;
            return ;
        }
        *AUX_MU_IO = buffer_pop(uart_out);
    }
}

static void timer_interrupt_handler() {
    resched_flag = true;
    if (remaining_time >= 0) {
        remaining_time--;
        if (!remaining_time)
            wake_up(1);
    }

    asm volatile("mrs    x1, cntfrq_el0");
    asm volatile("add    x0, xzr, x1, lsr #3");
    asm volatile("msr    cntp_tval_el0, x0");
}

void irq_route() {
    if ((*GPU_PEND1 & (1 << 29))) {
        uart_interrupt_handler();
    } else if ((*CORE0_IRQ_SRC & 0x2)) {
        timer_interrupt_handler();
    }
}
