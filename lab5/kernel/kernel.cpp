#include "mini_uart.h"

extern"C"
void kernel_exception() {
    io() << "Unknown exception\r\n";
}


extern"C"
void kernel_timer() {
    uint64_t cntpct_el0, cntfrq_el0;
    asm volatile(R"(
        mrs %x0, cntpct_el0
        mrs %x1, cntfrq_el0
    )":"=r"(cntpct_el0), "=r"(cntfrq_el0));
    io() << "經過" << (cntpct_el0 / cntfrq_el0) << "秒\r\n";
    asm volatile(R"(
        mrs x0, cntfrq_el0
        msr cntp_tval_el0, x0
    )":::"x0");
}
