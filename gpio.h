
#define MMIO_BASE 0x3F000000
#define GPFSEL1 ((volatile unsigned int*)(MMIO_BASE + 0x200004))
#define GPPUD ((volatile unsigned int*)(MMIO_BASE + 0x200094))
#define GPPUDCLK0 ((volatile unsigned int*)(MMIO_BASE + 0x200098))

