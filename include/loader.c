#include "loader.h"
#include "uart.h"
#include "utility.h"
BOOT_INFO boot_info __attribute__ ((aligned (16))) = {0};

unsigned long remote_load (unsigned long *base, unsigned long size) {
    unsigned long hex = 0, buf64 = 0;
    printf("%x", hex);

    for (unsigned long i = 0; i < size; i++) {
        char c = uart_getc();
        unsigned long tmp = 0;
        tmp |= c;
        buf64 |= tmp << ((i % 8) * 8);
        if (i % 8 == 7) {
            hex ^= buf64;
            *base++ = buf64;
            buf64 = 0;
            printf("%x", hex);
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
    printf("device tree address: %x", boot_info.device_tree_addr);
    printf("\ndevice tree size: %x", boot_info.device_tree_size);
    printf("\nbooting flag0: %x", boot_info.bootloader_flag[0]);
    printf("\nbooting flag1: %x", boot_info.bootloader_flag[1]);
    printf("\nbooting flag2: %x", boot_info.bootloader_flag[2]);
    printf("\nbootloader address: %x", boot_info.bootloader_addr);
    printf("\nbootloader size: %x", boot_info.bootloader_size);
    printf("\nkernel address: %x", boot_info.kernel_addr);
    printf("\nkernel size: %x", boot_info.kernel_size);
    printf("\nbss address: %x", boot_info.bss_addr);
    printf("\nbss size: %x", boot_info.bss_size);
    printf("\nstack base: %x", boot_info.stack_base);
    printf("\nstack size: %x", boot_info.stack_size);
    printf("\nstartup allocator address: %x", boot_info.startup_allocator_addr);
    printf("\ncpio address: %x", boot_info.cpio_addr);
    printf("\ncpio end: %x", boot_info.cpio_end);
    printf("\nbuddy system address: %x", boot_info.buddy_system_addr);
    printf("\nbuddy system size: %x\n", boot_info.buddy_system_size);
}
