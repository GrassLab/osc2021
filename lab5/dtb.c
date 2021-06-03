#include "include/dtb.h"
#include "include/util.h"

extern unsigned long DTB_HEADER;

unsigned int *find_next_token(unsigned int *token_addr) {
    switch (big_to_little_32(*token_addr)) {
        case FDT_NOP:
        case FDT_END_NODE:
            return token_addr + 1;

        case FDT_BEGIN_NODE: {
            char *name = ((ftd_node_header *)token_addr)->name;
            while (*name++);
            return (unsigned int *)align((unsigned long)name, 4);
        }

        case FDT_PROP: {
            void *value = (fdt_node_prop *)token_addr + 1;
            return (unsigned int *)align((unsigned long)value + big_to_little_32(((fdt_node_prop *)token_addr)->len), 4);
        }

        case FDT_END:
            return NULL;

        default:
            break;
    }
    return NULL;
}

unsigned long find_next_node(fdt_header *header, long node_addr, int *depth) {

    if (node_addr < 0) {
        (*depth)++;
        return (unsigned long)header + big_to_little_32(header->off_dt_struct);
    }

    unsigned int *token_addr = (unsigned int *)node_addr;

    do {
        token_addr = find_next_token(token_addr);

        switch (big_to_little_32(*token_addr)) {
            case FDT_NOP:
            case FDT_PROP:
                break;
                
            case FDT_END_NODE:
                (*depth)--;
                break;

            case FDT_BEGIN_NODE:
                (*depth)++;
                break;

            case FDT_END:
                return NULL;

            default:
                return NULL;
        }
    } while(big_to_little_32(*token_addr) != FDT_BEGIN_NODE);

    return ((unsigned long)token_addr);
}

int dtb_scan(int (*it)(fdt_header *header, unsigned long node_addr, int depth)) {
    fdt_header *header = (fdt_header *)DTB_HEADER;

    if (big_to_little_32(header->magic) != MAGIC_NUM)
        return -1;

    long node_addr;
    int depth = -1, rc = 0;

    for (node_addr = find_next_node(header, -1, &depth);
         node_addr && depth >= 0 && !rc;
         node_addr = find_next_node(header, node_addr, &depth)) {
        
        rc = it(header, node_addr, depth);
    }

    return rc;
}