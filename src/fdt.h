#ifndef FDT_H
#define FDT_H
#include "data_type.h"

#define FDT_BEGIN_NODE_BIG 0x1000000
#define FDT_END_NODE_BIG 0x2000000
#define FDT_PROP_BIG 0x3000000
#define FDT_NOP_BIG 0x4000000
#define FDT_END_BIG 0x9000000

typedef struct fdt_header {
    u32 magic;
    u32 totalsize;
    u32 off_dt_struct;
    u32 off_dt_strings;
    u32 off_mem_rsvmap;
    u32 version;
    u32 last_comp_version;
    u32 boot_cpuid_phys;
    u32 size_dt_strings;
    u32 size_dt_struct;
} FDT_HEADER;

typedef struct fdt_prop_header {
    u32 len;
    u32 nameoff;
} FDT_PROP_HEADER;

extern FDT_HEADER *fdt_head;

u32 u32_b2l (u32 num);

u32 get_fdt_header_magic ();
u32 get_fdt_header_totalsize ();
u32 get_fdt_header_off_dt_struct ();
u32 get_fdt_header_off_dt_strings ();
u32 get_fdt_header_off_mem_rsvmap ();
u32 get_fdt_header_version ();
u32 get_fdt_header_last_comp_version ();
u32 get_fdt_header_boot_cpuid_phys ();
u32 get_fdt_header_size_dt_strings ();
u32 get_fdt_header_size_dt_struct ();

void show_fdt_info ();
void show_all_fdt ();
void fdt_init ();
#endif
