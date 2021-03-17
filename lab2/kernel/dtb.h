#ifndef __DTB_H
#define __DTB_H

#include "miniuart.h"
#include "string.h"

#define DTB_MAGIC_NUM 0xd00dfeed
#define bswap32(num) __builtin_bswap32(num)
#define ROUNDUP_MUL4(num) ((num + 3) & ~0x3)

typedef struct {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;     //offset in bytes of the structure block
    uint32_t off_dt_strings;    //offset in bytes of the strings block
    uint32_t off_mem_rsvmap;    //offset in bytes of the memory reservation block
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;   //length in bytes of the strings block
    uint32_t size_dt_struct;    //length in bytes of the structure block
} fdt_header;

typedef struct {
    uint64_t address;
    uint64_t size;
} fdt_reserve_entry;

typedef struct {
    uint32_t len;
    uint32_t nameoff;
} fdt_prop_entry;

static int dtb_node_name_strcmp(const char *find, const char *from);
static void dtb_print_raw_token(const uint32_t *token);
static uint32_t dtb_print_node_name(const char *str, uint32_t printing);
static void dtb_print_string_list(const char *str, uint32_t len);
static void dtb_print_prop(const char *name, const uint32_t *token, uint32_t len);
static void dtb_traverse_node(const uint32_t *token, const char *str_blk, const char *find_node);
void dtb_parse(char *find_node);

#endif
