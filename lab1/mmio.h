#ifndef MMIO_H
#define MMIO_H

/*
Rpi3 uses mmio for peripheral registers. There is a VideoCore/ARM MMU translating physical addresses to bus addresses.
The MMU maps physical address 0x3f000000 to bus address 0x7e000000. 
You should use physical addresses instead of bus addresses.
*/

#define MMIO_BASE       0x3F000000
#endif