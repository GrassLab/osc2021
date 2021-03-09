#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__
#include "uart.h"

void bootloader_loadimg();
void copy_bootloader();
void load_new_kernel();

#endif