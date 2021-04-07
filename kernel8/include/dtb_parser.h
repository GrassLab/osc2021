#ifndef __DTB_PARSER_H__
#define __DTB_PARSER_H__
#include "uart.h"
#include "system.h"
#include "utils.h"

#define PADDING         0
#define FDT_BEGIN_NODE  1
#define FDT_END_NODE    2
#define FDT_PROP        3
#define FDT_NOP         4
#define FDT_END         9
#define DT_ADDR         0x31000000




struct fdt_header {
    unsigned int magic; // This field shall contain the value 0xd00dfeed (big-endian).
    unsigned int totalsize;
    unsigned int off_dt_struct;   // offset of the structure block from the beginning of the header.
    unsigned int off_dt_strings;  // offset of the strings block from the beginning of the header.
    unsigned int off_mem_rsvmap;  // offset of the mem.res. block from the beginning of the header.
    unsigned int version;
    unsigned int last_comp_version;
    unsigned int boot_cpuid_phys;
    unsigned int size_dt_strings; // length in bytes of the strings block section
    unsigned int size_dt_struct;  // length in bytes of the structure block section
};

unsigned int convert_big_to_small_endian(unsigned int num);
void print_dt_info();
void parse_dt();



#endif
