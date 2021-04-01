#include "loader.h"
#include "uart.h"
#include "utility.h"
BOOT_INFO boot_info __attribute__ ((aligned (16))) = {0};

unsigned long remote_load (unsigned long *base, unsigned long size) {
    unsigned long hex = 0, buf64 = 0;
    print("%x", hex);

    for (unsigned long i = 0; i < size; i++) {
        char c = uart_getc();
        unsigned long tmp = 0;
        tmp |= c;
        buf64 |= tmp << ((i % 8) * 8);
        if (i % 8 == 7) {
            hex ^= buf64;
            *base++ = buf64;
            buf64 = 0;
            print("%x", hex);
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
    print("device tree address: %x", boot_info.device_tree_addr);
    print("\ndevice tree size: %x", boot_info.device_tree_size);
    print("\nbooting flag0: %x", boot_info.bootloader_flag[0]);
    print("\nbooting flag1: %x", boot_info.bootloader_flag[1]);
    print("\nbooting flag2: %x", boot_info.bootloader_flag[2]);
    print("\nbootloader address: %x", boot_info.bootloader_addr);
    print("\nbootloader size: %x", boot_info.bootloader_size);
    print("\nkernel address: %x", boot_info.kernel_addr);
    print("\nkernel size: %x", boot_info.kernel_size);
    print("\nbss address: %x", boot_info.bss_addr);
    print("\nbss size: %x", boot_info.bss_size);
    print("\nstack base: %x", boot_info.stack_base);
    print("\nstack size: %x", boot_info.stack_size);
    print("\nstartup allocator address: %x", boot_info.startup_allocator_addr);
    print("\ncpio address: %x", boot_info.cpio_addr);
    print("\ncpio end: %x", boot_info.cpio_end);
    print("\nbuddy system address: %x", boot_info.buddy_system_addr);
    print("\nbuddy system size: %x\n", boot_info.buddy_system_size);
}
