#include "dtb_parser.h"
#include "driver.h"
extern char __dtb_addr[];
extern char __start_addr[];
extern char __end[];

struct init_driver driver_list[DRIVER_NUM] = {
    //{.name = "", .callback = __show_device_node_info},
    {.name = "brcm,bcm2835-gpio", .callback = bcm2835_gpio_probe},
    {.name = "arm,cortex-a53", .callback = arm_cortex_a53_probe},
    {.name = "RTX-3080", .callback = RTX_3080_probe},
    {.name = "", .callback = ALL_probe}
};

void dtb_init(char* args){
    struct fdt_header header;
    if(__check_dtb(&header) == 0) return;
    if(strlen(args) > 0)
        parse_dtb(driver_list[DRIVER_NUM - 1].name, driver_list[DRIVER_NUM - 1].callback);
    else{
        for(int i = 0; i < DRIVER_NUM - 1; ++i){
            parse_dtb(driver_list[i].name, driver_list[i].callback);
        }
    }
}
void parse_dtb(char* args, void* callback){
    struct fdt_header header;
    if(__check_dtb(&header) == 0) return;
    char* addr = (char*)(header.address + header.off_dt_struct);
    
    while(sys_get32bits(addr) != FDT_END ){
        //addr = unflatten_fdt(addr, &header, args, -1);
        unflatten_fdt(&addr, &header, args, -1, callback);
    }
}
void parse_node_property(char** addr, struct fdt_header* header, int depth){
    uint32_t token = sys_get32bits(*addr);    *addr += 4;
    uint32_t len = sys_get32bits(*addr);      *addr += 4;
    uint32_t nameoff = sys_get32bits(*addr);  *addr += 4;
    if(depth >= 0){
        __print_alignchar(' ', depth + 2); // uart_puts("Property: ");
        uart_puts((char*)(header->off_dt_strings + nameoff + header->address));
        uart_puts(" = ");
        char* name = (char*)(header->off_dt_strings + nameoff + header->address);
        if(strcmp("compatible", name) == 0 || strcmp("model", name) == 0
            || strcmp("status", name) == 0 || strcmp("name", name) == 0 || strcmp("device_type", name) == 0){
            int cnt = 0;
            for(uint32_t i = 0; i < len;++cnt){
                if(cnt > 0) uart_send(',');
                uart_send('\"');
                uart_puts(*addr + i);
                uart_send('\"');
                i += strlen(*addr + i) + 1;
                // else uart_send(addr[i]);
            }
            
            //uart_puts("\r\n");
        }
        else if(strcmp("phandle", name) == 0 || strcmp("#address-cells", name) == 0 || strcmp("#size-cells", name) == 0
                || strcmp("virtual-reg", name) == 0 || strcmp("interrupt-parent", name) == 0){
            uart_send('<');
            uart_printint(sys_get32bits(*addr));
            uart_send('>');
            // uart_printint(res);
            // uart_puts("\r\n");
        }
        else {
            uart_send('<');
            uart_send(' ');
            for(uint32_t i = 0;i < len; i += 4){
                uint32_t res = sys_get32bits(*addr + i);
                uart_printhex(res);
                uart_send(' ');
            }
            uart_send('>');
            //uart_puts("\r\n");
        }
        uart_puts("\r\n");
    }
    *addr += len + need_padding(len, 4);
    //return (char*)((uint32_t)addr + len + need_padding(len, 4));
}
short __check_compatible(char** addr, struct fdt_header* header, char* args){
    uint32_t token = sys_get32bits(*addr);    *addr += 4;
    uint32_t len = sys_get32bits(*addr);      *addr += 4;
    uint32_t nameoff = sys_get32bits(*addr);  *addr += 4;
    char* name = (char*)(header->off_dt_strings + nameoff + header->address);
    short res = 0;
    if(strlen(args) == 0) res = 1;
    else if(strcmp("compatible", name) == 0){
        for(uint32_t i = 0; i < len;){
            if(strcmp(args, *addr + i) == 0) res = 1;
            i += strlen(*addr + i) + 1;
            // else uart_send(addr[i]);
        }
        
    }
    *addr += len + need_padding(len, 4);
    return res;
}
void unflatten_fdt(char** addr, struct fdt_header* header, char* args, int depth, void (*callback)(char*, struct fdt_header*, int)){
    while(sys_get32bits(*addr) == FDT_NOP)
        addr += 4;
    uint32_t begin_tag = sys_get32bits(*addr); *addr += 4;
    if(begin_tag == FDT_BEGIN_NODE){
        char* begin_addr = *addr - 4;

        short flag = 0;
        __print_string_align(addr);
        while(sys_get32bits(*addr) == FDT_NOP)  *addr += 4;
        while(sys_get32bits(*addr) == FDT_PROP) {
            flag |= __check_compatible(addr, header, args);
            // parse_node_property(addr, header, depth);
        }
        
        //__show_device_node_info(begin_addr, header, depth + flag);
        callback(begin_addr, header, depth + flag);

        if(flag) depth = (depth >= 0) ? depth + 1 : 1;
        else depth = (depth >= 0) ? depth + 1 : depth;
        while(sys_get32bits(*addr) == FDT_BEGIN_NODE) /*addr = */unflatten_fdt(addr, header, args, depth, callback);
        while(sys_get32bits(*addr) == FDT_NOP) *addr += 4;
        if(sys_get32bits(*addr) == FDT_END_NODE ) *addr += 4;
    }
    //return addr;
}
void __show_device_node_info(char* node_addr, struct fdt_header* header, int depth){
    if(depth < 0) return;
    // guarantee *node_addr = BEGIN_NODE_TAG
    node_addr += 4;
    __print_alignchar('|', depth);
    uart_puts(node_addr);
    uart_puts(": {\r\n");

    __print_string_align(&node_addr);
    while(sys_get32bits(node_addr) == FDT_NOP)  node_addr += 4;
    while(sys_get32bits(node_addr) == FDT_PROP) /**addr =*/ parse_node_property(&node_addr, header, depth );
    //while(sys_get32bits(*addr) == FDT_BEGIN_NODE) /*addr = */unflatten_fdt(addr, header, args, depth + 1);
    __print_alignchar(' ', depth);
    uart_puts("}\r\n");
}

void __print_alignchar(char c, int depth){
    if(depth < 0) return;
    uart_send(' ');
    for(int i = 0; i < depth ; ++i){
        uart_send(c);
        for(int j = 0; j < 2; ++j)
            uart_send(' ');
    }
}
void __print_string_align(char** addr){
    //uart_puts(addr);
    int len = strlen(*addr); // exclude '\0'
    *addr += len + 1 + need_padding(len + 1, 4);
}
int __check_dtb(struct fdt_header* header){
    extract_fdt_header(header);
    if(header->magic != 0xd00dfeed){
        uart_puts("No device tree blob\r\n");
        return 0;
    }
    else return 1;
}
void extract_fdt_header(struct fdt_header* header){
    char *ptr = __dtb_addr;
    unsigned long int addr = 0;
    for(int i = 0, shift = 0 ; i < 8; ++i, shift += 8, ++ptr)
        addr += ((*ptr) << shift);
    char* fdt_addr = (char*)addr;

    header->address = addr;
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
    // uart_puts("magic: ");             uart_printhex(header->magic            );
    // uart_puts("totalsize: ");         uart_printint(header->totalsize        ); uart_puts("\r\n");
    // uart_puts("off_dt_struct: ");     uart_printhex(header->off_dt_struct    ); 
    // uart_puts("off_dt_strings: ");    uart_printhex(header->off_dt_strings   ); 
    // uart_puts("off_mem_rsvmap: ");    uart_printhex(header->off_mem_rsvmap   ); 
    // uart_puts("version: ");           uart_printint(header->version          ); uart_puts("\r\n");
    // uart_puts("last_comp_version: "); uart_printint(header->last_comp_version); uart_puts("\r\n");
    // uart_puts("boot_cpuid_phys: ");   uart_printhex(header->boot_cpuid_phys  ); 
    // uart_puts("size_dt_strings: ");   uart_printhex(header->size_dt_strings  ); 
    // uart_puts("size_dt_struct: ");    uart_printhex(header->size_dt_struct   ); 
}  