#ifndef _UART_BOOT_H
#define _UART_BOOT_H
#include "uart.h"

#define KERNEL_ADDR ((char*)0x80000)
#define TEMP_ADDR ((char*)0x60000)

void loadimg();
void load_new_kernel();

#endif /*_UART_BOOT_H */