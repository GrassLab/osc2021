#define buff_size 64
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define MOVE_BYTES 0x800000
extern char _end[];
extern char _start[];