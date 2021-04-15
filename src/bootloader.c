#include "bootloader.h"
#include "io.h"
#include "uart.h"
#include "utility.h"
BOOT_INFO boot_info __attribute__ ((aligned (16))) = {0};

unsigned long remote_load (unsigned long *base, unsigned long size) {
    unsigned long hex = 0, buf64 = 0;
    kprintf("%x", hex);

    for (unsigned long i = 0; i < size; i++) {
        char c = uart_getc();
        unsigned long tmp = 0;
        tmp |= c;
        buf64 |= tmp << ((i % 8) * 8);
        if (i % 8 == 7) {
            hex ^= buf64;
            *base++ = buf64;
            buf64 = 0;
            kprintf("%x", hex);
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
    kprintf("device tree address: %x", boot_info.device_tree_addr);
    kprintf("\ndevice tree size: %x", boot_info.device_tree_size);
    kprintf("\nbooting flag0: %x", boot_info.bootloader_flag[0]);
    kprintf("\nbooting flag1: %x", boot_info.bootloader_flag[1]);
    kprintf("\nbooting flag2: %x", boot_info.bootloader_flag[2]);
    kprintf("\nbootloader address: %x", boot_info.bootloader_addr);
    kprintf("\nbootloader size: %x", boot_info.bootloader_size);
    kprintf("\nkernel address: %x", boot_info.kernel_addr);
    kprintf("\nkernel size: %x", boot_info.kernel_size);
    kprintf("\nbss address: %x", boot_info.bss_addr);
    kprintf("\nbss size: %x", boot_info.bss_size);
    kprintf("\nstack base: %x", boot_info.stack_base);
    kprintf("\nstack size: %x", boot_info.stack_size);
    kprintf("\nstartup allocator address: %x", boot_info.startup_allocator_addr);
    kprintf("\ncpio address: %x", boot_info.cpio_addr);
    kprintf("\ncpio end: %x", boot_info.cpio_end);
    kprintf("\nbuddy system address: %x", boot_info.buddy_system_addr);
    kprintf("\nbuddy system size: %x\n", boot_info.buddy_system_size);
}
