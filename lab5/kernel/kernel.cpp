#include <mini_uart.h>
#include <string.h>

extern"C"
void kernel_exception(uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3, uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7) {
    uint64_t esr_el1, spsr_el1, elr_el1, sctlr_el1, el, spsel;
    char buffer[30];
    asm(R"(
        mrs %x0, esr_el1
        mrs %x1, spsr_el1
        mrs %x2, elr_el1
        mrs %x3, SCTLR_EL1
        mrs %x4, CurrentEL
        mrs %x5, SPSel
    )":"=r"(esr_el1), "=r"(spsr_el1), "=r"(elr_el1), "=r"(sctlr_el1), "=r"(el), "=r"(spsel));
    
    io() << "ESR_EL1 = " << u64tohex(esr_el1, buffer, sizeof(buffer)) << "\r\n";
    io() << "SPSR_EL1 = " << u64tohex(spsr_el1, buffer, sizeof(buffer)) << "\r\n";
    io() << "ELR_EL1 = " << u64tohex(elr_el1, buffer, sizeof(buffer)) << "\r\n";
    io() << "SCTLR_EL1 = " << u64tohex(sctlr_el1, buffer, sizeof(buffer)) << "\r\n";
    io() << "CurrentEL = " << u64tohex(el >> 2, buffer, sizeof(buffer)) << "\r\n";
    io() << "SPSel = " << u64tohex(spsel, buffer, sizeof(buffer)) << "\r\n";
    io() << "x0 = " << u64tohex(x0, buffer, sizeof(buffer)) << "\r\n";
    io() << "x1 = " << u64tohex(x1, buffer, sizeof(buffer)) << "\r\n";
    io() << "x2 = " << u64tohex(x2, buffer, sizeof(buffer)) << "\r\n";
    io() << "x3 = " << u64tohex(x3, buffer, sizeof(buffer)) << "\r\n";
    io() << "x4 = " << u64tohex(x4, buffer, sizeof(buffer)) << "\r\n";
    io() << "x5 = " << u64tohex(x5, buffer, sizeof(buffer)) << "\r\n";
    io() << "x6 = " << u64tohex(x6, buffer, sizeof(buffer)) << "\r\n";
    io() << "x7 = " << u64tohex(x7, buffer, sizeof(buffer)) << "\r\n";
    io() << "Unknown exception\r\n";
    while (true);
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
