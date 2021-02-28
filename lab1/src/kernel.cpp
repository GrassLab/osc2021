#include <kernel.h>
#include <mmio.h>

extern uint64_t* __bss_start__;
extern uint64_t* __bss_end__;

void Kernel::InitBss() {
    for (uint64_t *bss = __bss_start__; bss < __bss_end__; bss++) {
        *bss = 0;
    }
}

void Kernel::Reset(uint32_t tick) {
    MMIO::set(MMIOREG::PM_RSTC, MMIO::PM_PASSWORD | 0x20);
    MMIO::set(MMIOREG::PM_WDOG, MMIO::PM_PASSWORD | tick);
}
