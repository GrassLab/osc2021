#ifndef __DTB__
#define __DTB__

#define MAGIC_NUM           0xd00dfeed
#define FDT_BEGIN_NODE      0x01
#define FDT_END_NODE        0x02
#define FDT_PROP            0x03
#define FDT_NOP             0x04
#define FDT_END             0x09

typedef struct {
    unsigned int magic;
    unsigned int totalsize;
    unsigned int off_dt_struct;
    unsigned int off_dt_strings;
    unsigned int off_mem_rsvmap;
    unsigned int version;
    unsigned int last_comp_version;
    unsigned int boot_cpuid_phys;
    unsigned int size_dt_strings;
    unsigned int size_dt_struct;
} fdt_header;

typedef struct {
    unsigned long address;
    unsigned long size;
} fdt_reserve_entry;

typedef struct {
    unsigned int token;
    char name[0];
} ftd_node_header;

typedef struct {
    unsigned int token;
    unsigned int len;
    unsigned int nameoff;
    char value[0];
} fdt_node_prop;

int dtb_scan(int (*it)(fdt_header *header, unsigned long node_addr, int depth));
unsigned int *find_next_token(unsigned int *token_addr);

#endif