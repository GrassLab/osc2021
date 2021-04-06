#ifndef MM_H
#define MM_H
#include "data_type.h"
/*
 * 1 ~ 2: bootloader address
 * 3 ~ 4: kernel address
 * 5 ~ 6: device tree address
 * 7 ~ 8: cpio address
 */
extern u64 address_info[8] __attribute__ ((aligned (16)));

#endif
