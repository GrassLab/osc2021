#include "fdt.h"
#include "data_type.h"
#include "io.h"
#include "string.h"
#include "bootloader.h"
#include "mm.h"

FDT_HEADER *fdt_head = 0;

void fdt_init () {
    if (!boot_info.device_tree_addr)
        return;
    fdt_head = (FDT_HEADER *)boot_info.device_tree_addr;
    boot_info.device_tree_size = get_fdt_header_totalsize();
    startup_lock_memory(boot_info.device_tree_addr,
            boot_info.device_tree_addr + boot_info.device_tree_size);
}

u32 u32_b2l (u32 num) {
    u32 value = 0;
    value += (num & 0xFF) << 24;
    value += (num & 0xFF00) << 8;
    value += (num & 0xFF0000) >> 8;
    value += (num & 0xFF000000) >> 24;
    return value;
}

u32 get_fdt_header_magic () {
    return u32_b2l(fdt_head->magic);
}

u32 get_fdt_header_totalsize () {
    return u32_b2l(fdt_head->totalsize);
}

u32 get_fdt_header_off_dt_struct () {
    return u32_b2l(fdt_head->off_dt_struct);
}

u32 get_fdt_header_off_dt_strings () {
    return u32_b2l(fdt_head->off_dt_strings);
}

u32 get_fdt_header_off_mem_rsvmap () {
    return u32_b2l(fdt_head->off_mem_rsvmap);
}

u32 get_fdt_header_version () {
    return u32_b2l(fdt_head->version);
}

u32 get_fdt_header_last_comp_version () {
    return u32_b2l(fdt_head->last_comp_version);
}

u32 get_fdt_header_boot_cpuid_phys () {
    return u32_b2l(fdt_head->boot_cpuid_phys);
}

u32 get_fdt_header_size_dt_strings () {
    return u32_b2l(fdt_head->size_dt_strings);
}

u32 get_fdt_header_size_dt_struct () {
    return u32_b2l(fdt_head->size_dt_struct);
}

void show_fdt_info () {
    if (!fdt_head)
        return;
    kprintf("base address: %x\n", (unsigned long) fdt_head);
    kprintf("magic: %x\n", get_fdt_header_magic());
    kprintf("totalsize: %x\n", get_fdt_header_totalsize());
    kprintf("off_dt_struct: %x\n", get_fdt_header_off_dt_struct());
    kprintf("off_dt_strings: %x\n", get_fdt_header_off_dt_strings());
    kprintf("off_mem_rsvmap: %x\n", get_fdt_header_off_mem_rsvmap());
    kprintf("version: %x\n", get_fdt_header_version());
    kprintf("last_comp_version: %x\n", get_fdt_header_last_comp_version());
    kprintf("boot_cpuid_phys: %x\n", get_fdt_header_boot_cpuid_phys());
    kprintf("size_dt_strings: %x\n", get_fdt_header_size_dt_strings());
    kprintf("size_dt_struct: %x\n", get_fdt_header_size_dt_struct());
}

void kprintf_u32s (u32 *ptr, int num) {
    for (int i = 0; i < num; i++) {
        kprintf("%x ", u32_b2l(ptr[i]));
    }
}

u32 parse_prop (u32 offset, char is_kprintf) {
    unsigned long tmp = (unsigned long) fdt_head;
    u32 *fdt_struct = (u32 *)(tmp + get_fdt_header_off_dt_struct());
    char *fdt_string = (char *)(tmp + get_fdt_header_off_dt_strings());
    FDT_PROP_HEADER *header = (FDT_PROP_HEADER *)&fdt_struct[offset + 1];
    u32 len = u32_b2l(header->len);
    u32 nameoff = u32_b2l(header->nameoff);
    char *name = (char *)&fdt_string[nameoff];
    unsigned char *ptr = (unsigned char *)&fdt_struct[offset + 3];

    if (is_kprintf) {
        kprintf("%s: ", name);
        if (strlen((char *)ptr) != len - 1) {
            kprintf_u32s((u32 *)ptr, len / 4);
        }
        else
            kprintf((char *)&fdt_struct[offset + 3]);
        kprintf("\n");
    }

    return len % 4 ? len / 4 + 4 : len / 4 + 3;
}

void show_all_fdt () {
    unsigned long tmp = (unsigned long) fdt_head;
    u32 *fdt_struct = (u32 *)(tmp + get_fdt_header_off_dt_struct());
    char *fdt_string = (char *)(tmp + get_fdt_header_off_dt_strings());

    kprintf("%x\n", (unsigned long)fdt_struct);
    kprintf("%x\n", (unsigned long)fdt_string);

    /* start parse structure */
    for (int i = 0; fdt_struct[i] != FDT_END_BIG;) {
        /* next chunck is string */
        if (fdt_struct[i] == FDT_BEGIN_NODE_BIG) {
            kprintf("\n%s\n", (char *)&fdt_struct[i + 1]);
            unsigned long len = strlen((char *)&fdt_struct[i + 1]) + 1;
            i += len / 4;
            i += len % 4 ? 1 : 0;
            i++;
        }
        else if (fdt_struct[i] == FDT_END_NODE_BIG) {
            i++;
        }
        else if (fdt_struct[i] == FDT_PROP_BIG) {
            i += parse_prop(i, 1);
        }
        else if (fdt_struct[i] == FDT_NOP_BIG) {
            i++;
        }
        else {
            kprintf("error!!!!!!!\n");
            kprintf("%x\n", (unsigned long) &fdt_struct[i]);
            break;
        }
    }
}

