#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__
#include "uart.h"
#define KERNEL_ADDR ((char*)0x80000)
#define TMP_ADDR ((char*)0x100000)

void bootloader_loadimg();
void copy_bootloader();
void load_new_kernel();

#endif