#define buff_size 64
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define SHIFT_ADDR 0x100000
#define KERNEL_ADDR 0x4000000

extern char _end[];
extern char _start[];