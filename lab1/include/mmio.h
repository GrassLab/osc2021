#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

enum class MMIOREG {
    MMIO_BASE       = 0x3F000000,
    AUX_ENABLES     = MMIO_BASE + 0x215004,
    AUX_MU_IO_REG   = MMIO_BASE + 0x215040,
    AUX_MU_IER_REG  = MMIO_BASE + 0x215044,
    AUX_MU_IIR_REG  = MMIO_BASE + 0x215048,
    AUX_MU_LCR_REG  = MMIO_BASE + 0x21504C,
    AUX_MU_MCR_REG  = MMIO_BASE + 0x215050,
    AUX_MU_LSR_REG  = MMIO_BASE + 0x215054,
    AUX_MU_MSR_REG  = MMIO_BASE + 0x215058,
    AUX_MU_SCRATCH  = MMIO_BASE + 0x21505C,
    AUX_MU_CNTL_REG = MMIO_BASE + 0x215060,
    AUX_MU_STAT_REG = MMIO_BASE + 0x215064,
    AUX_MU_BAUD_REG = MMIO_BASE + 0x215068,
    GPFSEL1         = MMIO_BASE + 0x200004,
    GPPUD           = MMIO_BASE + 0x200094,
    GPPUDCLK0       = MMIO_BASE + 0x200098,
    PM_RSTC         = MMIO_BASE + 0x10001C,
    PM_WDOG         = MMIO_BASE + 0x100024
};

class MMIO {
public:
    static const uint32_t PM_PASSWORD     = 0x5A000000;
    static inline void set(MMIOREG addr, uint32_t val) {
        asm volatile("str %w[val], [%x[addr]]" :: [val]"r"(val), [addr]"r"(addr) : "memory");
        // Compiler would store 64-bit data with the following code with aarch64-linux-gnu-g++ on Ubuntu 20.04
        // So do not use it.
        // ((volatile uint32_t*)addr)[0] = val;
    }
    static inline uint32_t get(MMIOREG addr) {
        uint32_t ret;
        asm volatile("ldr %w[ret_val], [%x[addr]]" : [ret_val]"=r"(ret) : [addr]"r"(addr) : "memory");
        return ret;
        // Compiler would store 64-bit data with the following code with aarch64-linux-gnu-g++ on Ubuntu 20.04
        // So do not use it.
        // return *(volatile uint32_t*)addr;
    }
};

#endif
