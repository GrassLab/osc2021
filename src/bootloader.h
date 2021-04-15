#ifndef BOOTLOADER_H
#define BOOTLOADER_H
#include "data_type.h"

typedef struct boot_information {
    u64 device_tree_addr;
    u64 bootloader_flag[3];
    u64 bootloader_addr;
    u64 bootloader_size;
    u64 kernel_addr;
    u64 kernel_size;
    u64 bss_addr;
    u64 bss_size;
    u64 stack_base;
    u64 stack_size;
    u64 startup_allocator_addr;
    u64 cpio_addr;
    u64 cpio_end;
    u64 device_tree_size;
    u64 buddy_system_addr;
    u64 buddy_system_size;
} BOOT_INFO;
extern BOOT_INFO boot_info;

unsigned long remote_load (unsigned long *base, unsigned long size);
int relocate_process (u64 *source);
void show_boot_info ();

#endif
