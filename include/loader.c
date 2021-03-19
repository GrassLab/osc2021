#include "loader.h"
#include "uart.h"
#include "utility.h"

u64 bootloader_info[4] __attribute__ ((aligned (16)));

unsigned long remote_load (unsigned long *base, unsigned long size) {
    unsigned long hex = 0, buf64 = 0;
    uart_sendh(hex);
    uart_send("\r\n");

    for (unsigned long i = 0; i < size; i++) {
        char c = uart_getc();
        unsigned long tmp = 0;
        tmp |= c;
        buf64 |= tmp << ((i % 8) * 8);
        if (i % 8 == 7) {
            hex ^= buf64;
            *base++ = buf64;
            buf64 = 0;
            uart_sendh(hex);
            uart_send("\r\n");
        }
    }
    return hex;
}

int relocate_process () {
    unsigned long *destination = (unsigned long *)get_bootloader_start();
    unsigned long *source = (unsigned long *)0x80000;
    unsigned long size = get_bootloader_end() - get_bootloader_start() + 8;
    for (int i = 0; i < size / 8; i++) {
        *destination++ = *source++;
    }
    return 1;
}

int clear_memory () {
    unsigned long *source = (unsigned long *)0x80000;
    unsigned long size = get_bootloader_end() - get_bootloader_start() + 8;
    for (int i = 0; i < size / 8; i++) {
        *source++ = 0;
    }
    return 1;
}
