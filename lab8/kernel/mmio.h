#ifndef MMIO_H
#define MMIO_H

#include "mmu.h"

//rpi3 access peripheral registers by memory mapped io (MMIO).
//There is a VideoCore/ARM MMU sit between ARM CPU and peripheral bus. 
//This MMU maps ARM’s physical address 0x3f000000 to 0x7e000000.

#define MMIO_BASE       (0x3F000000 + KVA)

#endif