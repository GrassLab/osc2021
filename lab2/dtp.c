#include "include/dtp.h"
#include "include/cutils.h"
#define FDT_BEGIN_NODE 0x00000001 // each node begins with FDT_BEGIN_NODE
#define FDT_END_NODE 0x00000002   // end with FDT_END_NODE. Node’s properties and subnodes are before the FDT_END_NOD
#define FDT_PROP 0x00000003 // describe property
#define FDT_NOP 0x00000004
#define FDT_END 0x00000009 //  end of the structure block

char *dt_base_g = (char*)1; // Set to a non-zero value, so dt_base_g won't be in .bss

typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
struct fdt_header {
    uint32_t magic; // This field shall contain the value 0xd00dfeed (big-endian).
    uint32_t totalsize;
    uint32_t off_dt_struct;   // offset of the structure block from the beginning of the header.
    uint32_t off_dt_strings;  // offset of the strings block from the beginning of the header.
    uint32_t off_mem_rsvmap;  // offset of the mem.res. block from the beginning of the header.
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings; // length in bytes of the strings block section
    uint32_t size_dt_struct;  // length in bytes of the structure block section
};

struct fdt_reserve_entry {
    uint64_t address;
    uint64_t size;
};



uint32_t get_uint_endian(unsigned int *addr)
{ // Used to parse uint32_t in device tree object
    unsigned char *byte_ptr;

    // byte_ptr = (unsigned char*)(&(header->magic));
    byte_ptr = (unsigned char*)addr;
    unsigned int res = ((unsigned int)*byte_ptr << 24)  +
                        ((unsigned int)*(byte_ptr + 1) << 16)  +
                        ((unsigned int)*(byte_ptr + 2) << 8) +
                        ((unsigned int)*(byte_ptr + 3));
    return res;
}

uint64_t get_ulong_endian(unsigned long *addr)
{ // Used to parse uint32_t in device tree object
    unsigned char *byte_ptr;

    // byte_ptr = (unsigned char*)(&(header->magic));
    byte_ptr = (unsigned char*)addr;
    unsigned long res = ((unsigned long)*byte_ptr << 56)  +
                        ((unsigned long)*(byte_ptr + 1) << 48) +
                        ((unsigned long)*(byte_ptr + 2) << 40) +
                        ((unsigned long)*(byte_ptr + 3) << 32) +
                        ((unsigned long)*(byte_ptr + 4) << 24) +
                        ((unsigned long)*(byte_ptr + 5) << 16) +
                        ((unsigned long)*(byte_ptr + 6) << 8) +
                        ((unsigned long)*(byte_ptr + 7));
    return res;
}

void print_indent(unsigned int level)
{
    for (int i = 0; i < level; ++i)
        uart_send_string("    ");
    return;
}

// int do_dtp()
// {
//     // unsigned long dt_base = 0x80000;
//     char *dt_struct_ptr, *prop_data_end;
//     unsigned int token, prop_data_len, prop_name_off, dt_level;
//     // struct fdt_reserve_entry *res_ent;

//     uart_send_long((unsigned long)dt_base_g);
//     uart_send_string("\r\n");
//     struct fdt_header *header = (struct fdt_header*)dt_base_g;

//     uart_send_string("Device tree magic: ");
//     uart_send_uint(get_uint_endian(&(header->magic)));
//     uart_send_string("\r\n");
//     uart_send_string("Device tree version: ");
//     uart_send_uint(get_uint_endian(&(header->version)));
//     uart_send_string("\r\n");
//     char *dt_memres_base = dt_base_g + get_uint_endian(&(header->off_mem_rsvmap));
//     char *dt_struct_base = dt_base_g + get_uint_endian(&(header->off_dt_struct));
//     char *dt_struct_end = dt_struct_base + get_uint_endian(&(header->size_dt_struct));
//     char *dt_strings_base = dt_base_g + get_uint_endian(&(header->off_dt_strings));
//     uart_send_string("string base: ");
//     uart_send_string(dt_strings_base);
//     uart_send_string("\r\n");
    
//     // res_ent = (struct fdt_reserve_entry*)dt_memres_base;
//     // uart_send_string("ALLOHA: \r\n");
//     // uart_send_ulong(get_ulong_endian(&(res_ent->address)));
//     // uart_send_string("\r\n");
//     // uart_send_ulong(get_ulong_endian(&(res_ent->size)));
//     // uart_send_string("\r\n");
//     // res_ent++;
//     // uart_send_ulong(get_ulong_endian(&(res_ent->address)));
//     // uart_send_string("\r\n");
//     // uart_send_ulong(get_ulong_endian(&(res_ent->size)));
//     // uart_send_string("\r\n");
//     char *res_ent;
//     res_ent = dt_memres_base;
//     uart_send_string("ALLOHA: \r\n");
//     uart_send_ulong(get_ulong_endian((uint64_t*)res_ent));
//     res_ent += sizeof(uint64_t);
//     uart_send_string("\r\n");
//     uart_send_ulong(get_ulong_endian((uint64_t*)res_ent));
//     uart_send_string("\r\n");
//     res_ent += sizeof(uint64_t);
//     uart_send_ulong(get_ulong_endian((uint64_t*)res_ent));
//     res_ent += sizeof(uint64_t);
//     uart_send_string("\r\n");
//     uart_send_ulong(get_ulong_endian((uint64_t*)res_ent));
//     uart_send_string("\r\n");
//     dt_level = 0;
//     dt_struct_ptr = dt_struct_base;
//     while (dt_struct_ptr < dt_struct_end) {
//         token = get_uint_endian((unsigned int*)dt_struct_ptr);
//         dt_struct_ptr += sizeof(unsigned int);
//         switch (token) {
//             case FDT_BEGIN_NODE:
//                 print_indent(dt_level);
//                 uart_send_string("FDT_BEGIN_NODE\r\n");
//                 print_indent(dt_level);
//                 uart_send_string(dt_struct_ptr);
//                 uart_send_string("\r\n");
//                 dt_struct_ptr = align_upper(dt_struct_ptr + strlen(dt_struct_ptr) + 1, 4);
//                 dt_level++;
//                 break;
//             case FDT_END_NODE:
//                 dt_level--;
//                 print_indent(dt_level);
//                 uart_send_string("FDT_END_NODE\r\n");
//                 break;
//             case FDT_PROP:
//                 print_indent(dt_level);
//                 uart_send_string("FDT_PROP\r\n");
//                 prop_data_len = get_uint_endian((unsigned int*)dt_struct_ptr);
//                 dt_struct_ptr += sizeof(unsigned int);
//                 prop_name_off = get_uint_endian((unsigned int*)dt_struct_ptr);
//                 dt_struct_ptr += sizeof(unsigned int);
//                 prop_data_end = dt_struct_ptr + prop_data_len;
//                 print_indent(dt_level);
//                 uart_send_string(dt_strings_base + prop_name_off);
//                 uart_send_string(" : ");
//                 for (; dt_struct_ptr < prop_data_end; dt_struct_ptr++)
//                     uart_send(*dt_struct_ptr);
//                 uart_send_string("\r\n");
//                 dt_struct_ptr = align_upper(dt_struct_ptr, 4);
//                 break;
//             case FDT_NOP:
//                 print_indent(dt_level);
//                 uart_send_string("FDT_NOP\r\n");
//                 break;
//             case FDT_END:
//                 print_indent(dt_level);
//                 uart_send_string("FDT_END\r\n");
//                 break;
//             default:
//                 uart_send_string("Unrecognized token.\r\n");
//                 return 1;
//         }
//     }
//     return 0;
// }


int do_dtp(int (*_probe_func)(struct dtn *node))
{
    // unsigned long dt_base = 0x80000;
    char buf[128], *buf_ptr;
    char *dt_struct_ptr, *prop_data_end, *node_name, *prop_name;
    unsigned int token, prop_data_len, prop_name_off, dt_level;
    struct dtn node;
    struct fdt_header *header;

    header = (struct fdt_header*)dt_base_g;
    char *dt_memres_base = dt_base_g + get_uint_endian(&(header->off_mem_rsvmap));
    char *dt_struct_base = dt_base_g + get_uint_endian(&(header->off_dt_struct));
    char *dt_struct_end = dt_struct_base + get_uint_endian(&(header->size_dt_struct));
    char *dt_strings_base = dt_base_g + get_uint_endian(&(header->off_dt_strings));

    
    dt_level = 0;
    dt_struct_ptr = dt_struct_base;
    while (dt_struct_ptr < dt_struct_end) {
        buf_ptr = buf;
        token = get_uint_endian((unsigned int*)dt_struct_ptr);
        dt_struct_ptr += sizeof(unsigned int);
        switch (token) {
            case FDT_BEGIN_NODE:
                node.name = dt_struct_ptr;
                dt_struct_ptr = align_upper(dt_struct_ptr + strlen(dt_struct_ptr) + 1, 4);
                break;
            case FDT_END_NODE:
                break;
            case FDT_PROP:
                prop_data_len = get_uint_endian((unsigned int*)dt_struct_ptr);
                dt_struct_ptr += sizeof(unsigned int);
                prop_name_off = get_uint_endian((unsigned int*)dt_struct_ptr);
                dt_struct_ptr += sizeof(unsigned int);
                prop_data_end = dt_struct_ptr + prop_data_len;
                prop_name = dt_strings_base + prop_name_off;
                for (; dt_struct_ptr < prop_data_end; dt_struct_ptr++, buf_ptr++)
                    *buf_ptr = *dt_struct_ptr;
                *buf_ptr = '\0';
                dt_struct_ptr = align_upper(dt_struct_ptr, 4);
                if (!strcmp_with_len("compatible", prop_name, 10)) {
                    node.compatible = buf;
                    if (!(_probe_func(&node)))
                        return 0;
                }
                // if (!strcmp_with_len("compatible", prop_name, 10)){
                //     if (strstr(buf, "uart")) {
                //         uart_send_string(node_name);
                //         uart_send_string("\r\n    ");
                //         uart_send_string(buf);
                //         uart_send_string("\r\n");
                //     }
                // }
                break;
            case FDT_NOP:
                break;
            case FDT_END:
                break;
            default:
                uart_send_string("Unrecognized token.\r\n");
                return 1;
        }

    }
    return 0;
}