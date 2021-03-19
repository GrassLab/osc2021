#include "flattened_devicetree.h"
#include "data_type.h"
#include "uart.h"
#include "string.h"

FDT_HEADER *fdt_head;

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
    uart_send("base address: "); uart_sendh((unsigned long) fdt_head); uart_send("\r\n");
    uart_send("magic: "); uart_sendh(get_fdt_header_magic()); uart_send("\r\n");
    uart_send("totalsize: "); uart_sendh(get_fdt_header_totalsize()); uart_send("\r\n");
    uart_send("off_dt_struct: "); uart_sendh(get_fdt_header_off_dt_struct()); uart_send("\r\n");
    uart_send("off_dt_strings: "); uart_sendh(get_fdt_header_off_dt_strings()); uart_send("\r\n");
    uart_send("off_mem_rsvmap: "); uart_sendh(get_fdt_header_off_mem_rsvmap()); uart_send("\r\n");
    uart_send("version: "); uart_sendh(get_fdt_header_version()); uart_send("\r\n");
    uart_send("last_comp_version: "); uart_sendh(get_fdt_header_last_comp_version()); uart_send("\r\n");
    uart_send("boot_cpuid_phys: "); uart_sendh(get_fdt_header_boot_cpuid_phys()); uart_send("\r\n");
    uart_send("size_dt_strings: "); uart_sendh(get_fdt_header_size_dt_strings()); uart_send("\r\n");
    uart_send("size_dt_struct: "); uart_sendh(get_fdt_header_size_dt_struct()); uart_send("\r\n");
}

void print_u32s (u32 *ptr, int num) {
    for (int i = 0; i < num; i++) {
        uart_sendh(u32_b2l(ptr[i]));
        uart_send(" ");
    }
}

u32 parse_prop (u32 offset, char is_print) {
    unsigned long tmp = (unsigned long) fdt_head;
    u32 *fdt_struct = (u32 *)(tmp + get_fdt_header_off_dt_struct());
    char *fdt_string = (char *)(tmp + get_fdt_header_off_dt_strings());
    FDT_PROP_HEADER *header = (FDT_PROP_HEADER *)&fdt_struct[offset + 1];
    u32 len = u32_b2l(header->len);
    u32 nameoff = u32_b2l(header->nameoff);
    char *name = (char *)&fdt_string[nameoff];
    unsigned char *ptr = (unsigned char *)&fdt_struct[offset + 3];

    if (is_print) {
        uart_send(name);
        uart_send(": ");
        if (strlength((char *)ptr) != len - 1) {
            print_u32s((u32 *)ptr, len / 4);
        }
        else
            uart_send((char *)&fdt_struct[offset + 3]);
        uart_send("\r\n");
    }

    return len % 4 ? len / 4 + 4 : len / 4 + 3;
}

void show_all_fdt () {
    unsigned long tmp = (unsigned long) fdt_head;
    u32 *fdt_struct = (u32 *)(tmp + get_fdt_header_off_dt_struct());
    char *fdt_string = (char *)(tmp + get_fdt_header_off_dt_strings());

    uart_sendh((unsigned long)fdt_struct);
    uart_send("\r\n");
    uart_sendh((unsigned long)fdt_string);
    uart_send("\r\n");

    /* start parse structure */
    for (int i = 0; fdt_struct[i] != FDT_END_BIG;) {
        /* next chunck is string */
        if (fdt_struct[i] == FDT_BEGIN_NODE_BIG) {
            uart_send("\r\n");
            //uart_send("-->node start\r\n");
            uart_send((char *)&fdt_struct[i + 1]);
            uart_send("\r\n");
            unsigned long len = strlength((char *)&fdt_struct[i + 1]) + 1;
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
            uart_send("error!!!!!!!\r\n");
            uart_sendh((unsigned long) &fdt_struct[i]);
            uart_send("\r\n");
            break;
        }
    }
}

