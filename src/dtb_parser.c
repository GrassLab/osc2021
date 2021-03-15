#include "dtb_parser.h"
extern char __dtb_addr[];
extern char __start_addr[];
extern char __end[];

void parse_dtb(char* args){
    // 0x8000000 - 0x8000003
    char *ptr = __dtb_addr;
    unsigned long int fdt_addr = 0;
    for(int i = 0, shift = 0 ; i < 8; ++i, shift += 8, ++ptr)
        fdt_addr += ((*ptr) << shift);
    uart_puts("FDT address: ");
    uart_printhex(fdt_addr);
    struct fdt_header header;
    extract_fdt_header((char*)fdt_addr, &header);
    header.address = fdt_addr;
    // memory reservation block
    struct fdt_reserve_entry rsv_mem = {
        (char*)(fdt_addr + header.off_mem_rsvmap), 
        sys_get64bits((char*)(fdt_addr + header.off_mem_rsvmap + 8))};
    for(char* tmp = header.address + header.off_dt_strings;
    tmp < header.address + header.totalsize; ++tmp){
        uart_send(*tmp);
    }
    char* addr = (char*)(fdt_addr + header.off_dt_struct);
    while(sys_get32bits(addr) != FDT_END){
        addr = unflatten_fdt(addr, &header);
    }
}
char* parse_node_property(char* addr, struct fdt_header* header){
    uint32_t token = sys_get32bits(addr); addr += 4;
    // uart_printhex(token);
    uint32_t len = sys_get32bits(addr); addr += 4;
    uint32_t nameoff = sys_get32bits(addr); addr += 4;
    //uart_printint(nameoff);
    uart_puts("name: ");
    uart_puts((char*)(header->off_dt_strings + nameoff + header->address));
    uart_puts("\r\nproperty value: ");
    for(uint32_t i = 0; i < len; ++i){
        uart_send(*addr);
        ++addr;
    }
    uart_puts("\r\n");
    
    addr += (need_padding(len, 4));
    return addr + 4;
}
char* unflatten_fdt(char* addr, struct fdt_header* header){
    while(sys_get32bits(addr) == FDT_NOP)
        addr += 4;
    uint32_t begin_tag = sys_get32bits(addr); addr += 4;
    if(begin_tag == FDT_BEGIN_NODE){
        uart_puts("node name: ");
        uart_puts(addr);
        uart_puts("\r\n");
        addr = __align_byte(addr);
        while(sys_get32bits(addr) == FDT_NOP)
            addr += 4;
        while(sys_get32bits(addr) == FDT_PROP){
            addr = parse_node_property(addr, header);
        }
        uart_printhex(sys_get32bits(addr));
        if(sys_get32bits(addr) == FDT_BEGIN_NODE){
            addr = unflatten_fdt(addr, header);
            while(sys_get32bits(addr) == FDT_NOP)
                addr += 4;
        }
        if(sys_get32bits(addr) == FDT_END_NODE){
            addr += 4;
        }
        return addr;
    }
    else {
        uart_printhex(sys_get32bits(addr));
        uart_puts("not begin\r\n");

    }
    return addr + 4;
}
char* __align_byte(char* addr){
    short int cnt = 1;
    while(*addr != '\0'){
        cnt = (cnt + 1)%4;
        ++addr;
    }
    while(cnt != 0){
        cnt = (cnt + 1)%4;
        ++addr; 
    }
    return addr + 1;
}
void extract_fdt_header(char* fdt_addr, struct fdt_header* header){
    header->magic               = sys_get32bits(fdt_addr);
    header->totalsize           = sys_get32bits(fdt_addr + 4);
    header->off_dt_struct       = sys_get32bits(fdt_addr + 8);
    header->off_dt_strings      = sys_get32bits(fdt_addr + 12);
    header->off_mem_rsvmap      = sys_get32bits(fdt_addr + 16);
    header->version             = sys_get32bits(fdt_addr + 20);
    header->last_comp_version   = sys_get32bits(fdt_addr + 24);
    header->boot_cpuid_phys     = sys_get32bits(fdt_addr + 28);
    header->size_dt_strings     = sys_get32bits(fdt_addr + 32);
    header->size_dt_struct      = sys_get32bits(fdt_addr + 36);
    uart_puts("magic: ");             uart_printhex(header->magic            );
    uart_puts("totalsize: ");         uart_printint(header->totalsize        ); uart_puts("\r\n");
    uart_puts("off_dt_struct: ");     uart_printhex(header->off_dt_struct    ); 
    uart_puts("off_dt_strings: ");    uart_printhex(header->off_dt_strings   ); 
    uart_puts("off_mem_rsvmap: ");    uart_printhex(header->off_mem_rsvmap   ); 
    uart_puts("version: ");           uart_printint(header->version          ); uart_puts("\r\n");
    uart_puts("last_comp_version: "); uart_printint(header->last_comp_version); uart_puts("\r\n");
    uart_puts("boot_cpuid_phys: ");   uart_printhex(header->boot_cpuid_phys  ); 
    uart_puts("size_dt_strings: ");   uart_printhex(header->size_dt_strings  ); 
    uart_puts("size_dt_struct: ");    uart_printhex(header->size_dt_struct   ); 
}  