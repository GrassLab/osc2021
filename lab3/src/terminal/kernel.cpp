#include <kernel.h>
#include <mmio.h>

void Kernel::Reset(uint32_t tick) {
    MMIO::set(MMIOREG::PM_RSTC, MMIO::PM_PASSWORD | 0x20);
    MMIO::set(MMIOREG::PM_WDOG, MMIO::PM_PASSWORD | tick);
}
