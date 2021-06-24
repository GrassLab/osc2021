#ifndef DEVICETREEH
#define DEVICETREEH

#include "mmu.h"

#define DTB_ADDR        0x9000000	// dtb start address
#define DTB_MAGIC		0xd00dfeed	// magic value

#define FDT_BEGIN_NODE  1			// start node
#define FDT_END_NODE    2			// end node
#define FDT_PROP        3			// property
#define FDT_NOP         4			// nop
#define FDT_END         9			// dtb end

struct fdt_header 
{
    unsigned int magic;				// magic word FDT_MAGIC
    unsigned int totalsize;			// total size of DT block
    unsigned int off_dt_struct;		// offset to structure
    unsigned int off_dt_strings;	// offset to strings
    unsigned int off_mem_rsvmap;	// offset to memory reserve map
    unsigned int version;			// format version (17)
    unsigned int last_comp_version;	// last compatible version
    unsigned int boot_cpuid_phys;	// Which physical CPU id we're booting on
    unsigned int size_dt_strings;	// size of the strings block
    unsigned int size_dt_struct;	// size of the structure block	
};

typedef void (*dtb_callback)(unsigned long int ptr, unsigned long int strptr);

unsigned long int align4(unsigned long int size);
unsigned int convert_bigendian(void *ptr); // convert char to bigendian
void dtb_ls(); // list rootfs file
void parse_node(struct fdt_header *header, unsigned long int dtb);
int dtb_cat(char* name, dtb_callback cb); // get file content
int parse_and_callback(struct fdt_header *header, unsigned long int dtb, char* name, dtb_callback cb);
void print_indent(int level);

#endif
