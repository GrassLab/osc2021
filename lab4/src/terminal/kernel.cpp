#include <kernel.h>
#include <mmio.h>
#include <mini_uart.h>

void Kernel::Reset(uint32_t tick) {
    MMIO::set(MMIOREG::PM_RSTC, MMIO::PM_PASSWORD | 0x20);
    MMIO::set(MMIOREG::PM_WDOG, MMIO::PM_PASSWORD | tick);
}


void print_exception_reg() {
    uint64_t spsr_el1, elr_el1, esr_el1;
    asm volatile(R"(
        mrs %x[spsr_el1], spsr_el1
        mrs %x[elr_el1], elr_el1
        mrs %x[esr_el1], esr_el1
    )":[spsr_el1]"=r"(spsr_el1), [elr_el1]"=r"(elr_el1), [esr_el1]"=r"(esr_el1));
    IO() << "spsr_el1: " << spsr_el1 << "\r\n";
    IO() << "elr_el1: " << elr_el1 << "\r\n";
    IO() << "esr_el1: " << esr_el1 << "\r\n";
}

static uint64_t counter = 0;

void timer_handler() {
    IO() << "經過" << (++counter) << "秒\r\n";
    asm volatile(R"(
        mrs x0, cntfrq_el0
        msr cntp_tval_el0, x0
    )":::"x0");
}

void timer_enable() {
    asm volatile(R"(
        mov x0, 1
        msr cntp_ctl_el0, x0 // enable
        mrs x0, cntfrq_el0
        msr cntp_tval_el0, x0 // set expired time
        mov x0, 0
        msr spsr_el1, x0
    )":::"x0");
    MMIO::set(MMIOREG::CORE0_TIMER_IRQ_CTRL, 2);
}

void timer_disable() {
    asm volatile(R"(
        mov x0, 0x3c0
        msr spsr_el1, x0
    )":::"x0");
    MMIO::set(MMIOREG::CORE0_TIMER_IRQ_CTRL, 0);
}

extern "C"
void on_exception() {
    uint64_t esr_el1, spsr_el1;
    asm volatile("mrs %x[esr_el1], esr_el1\n mrs %x[spsr_el1], spsr_el1":[esr_el1]"=r"(esr_el1), [spsr_el1]"=r"(spsr_el1));
    uint64_t ec = esr_el1 >> 26;
    uint64_t iss = esr_el1 & 0x1ffffff;
    if (ec == 21) {
        if (iss == 1) {
            print_exception_reg();
        }
        if (iss == 2) {
            if ((spsr_el1 & 0x3c0) == 0) {
                timer_handler();
            }
            else {
                counter = 0;
                timer_enable();
            }
        }
        else if (iss == 3) {
            timer_disable();
        }
    }
}
