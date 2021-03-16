#ifndef __DTB_PARSER_H__
#define __DTB_PARSER_H__
#include "uart.h"
#include "system.h"
#include "string.h"


#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE 0x00000002
#define FDT_PROP 0x00000003
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009

struct fdt_header {
    uint32_t address;
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};
struct fdt_reserve_entry{
    char* address;
    uint64_t size;
};
struct fdt_tag_header{
    uint32_t tag;
    char* name;
};
struct node_property{
    struct property* next_property;
    uint32_t len;
    uint32_t nameoff;
};

struct dt_node{
    struct dt_node* child;
    char* name;
    struct node_property* property;
};

void parse_dtb(char*);
void extract_fdt_header(char*, struct fdt_header*);
char* unflatten_fdt(char* addr, struct fdt_header* header, int depth);
char* parse_node_property(char* addr, struct fdt_header* header, int depth);
char* __print_string_align(char* addr);
void __print_alignchar(char c, int depth);
#endif