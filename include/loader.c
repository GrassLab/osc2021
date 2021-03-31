#include "loader.h"
#include "uart.h"
#include "utility.h"
BOOT_INFO boot_info __attribute__ ((aligned (16))) = {0};

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

int relocate_process (u64 *source) {
    u64 *destination = (u64 *)boot_info.bootloader_addr;
    u64 size = boot_info.bootloader_size;
    for (int i = 0; i < size / 8; i++) {
        *destination++ = *source++;
    }
    return 1;
}

void show_boot_info () {
    uart_send("device tree address: ");
    uart_sendh(boot_info.device_tree_addr);

    uart_send("\r\ndevice tree size: ");
    uart_sendh(boot_info.device_tree_size);

    uart_send("\r\nbooting flag0: ");
    uart_sendh(boot_info.bootloader_flag[0]);

    uart_send("\r\nbooting flag1: ");
    uart_sendh(boot_info.bootloader_flag[1]);

    uart_send("\r\nbooting flag2: ");
    uart_sendh(boot_info.bootloader_flag[2]);

    uart_send("\r\nbootloader address: ");
    uart_sendh(boot_info.bootloader_addr);

    uart_send("\r\nbootloader size: ");
    uart_sendh(boot_info.bootloader_size);

    uart_send("\r\nkernel address: ");
    uart_sendh(boot_info.kernel_addr);

    uart_send("\r\nkernel size: ");
    uart_sendh(boot_info.kernel_size);

    uart_send("\r\nbss address: ");
    uart_sendh(boot_info.bss_addr);

    uart_send("\r\nbss size: ");
    uart_sendh(boot_info.bss_size);

    uart_send("\r\nstack base: ");
    uart_sendh(boot_info.stack_base);

    uart_send("\r\nstack size: ");
    uart_sendh(boot_info.stack_size);

    uart_send("\r\nstartup allocator address: ");
    uart_sendh(boot_info.startup_allocator_addr);

    uart_send("\r\ncpio address: ");
    uart_sendh(boot_info.cpio_addr);

    uart_send("\r\ncpio end: ");
    uart_sendh(boot_info.cpio_end);

    uart_send("\r\nbuddy system address: ");
    uart_sendh(boot_info.buddy_system_addr);

    uart_send("\r\nbuddy system size: ");
    uart_sendh(boot_info.buddy_system_size);

    uart_send("\r\n");
}
