#include "stdint.h"
#include "mini_uart.h"
#include "str_tool.h"

#define PADDING         0
#define FDT_BEGIN_NODE  1
#define FDT_END_NODE    2
#define FDT_PROP        3
#define FDT_NOP         4
#define FDT_END         9
#define DT_ADDR         0x100000

struct fdt_header {
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

uint32_t convert_big_to_small_endian(uint32_t num){
    return (num>>24&0xff) | (num<<8&0xff0000) | (num>>8&0xff00) | (num<<24&0xff000000);
}

uint32_t iter_one_struct(uint64_t *addr){
    *addr += 4;
    return convert_big_to_small_endian(*(uint32_t*)(*addr-4));
}

char* iter_one_string(uint64_t *addr){
    char *string_ptr = (char*)*addr;
    uint32_t string_len = strlen(string_ptr);
    if(string_len == 0)
        *addr += 1;
    else
        *addr += strlen(string_ptr);
    return string_ptr;
}

void print_dt_info(){
    uint64_t addr = DT_ADDR;
    uint64_t *dt_addr = (uint64_t*)addr;
    struct fdt_header* header = (struct fdt_header*)*dt_addr;

    uint32_t totalsize, off_dt_struct, off_dt_strings, size_dt_strings, size_dt_struct;
    totalsize = convert_big_to_small_endian(header->totalsize);
    off_dt_struct = convert_big_to_small_endian(header->off_dt_struct);
    off_dt_strings = convert_big_to_small_endian(header->off_dt_strings);
    size_dt_strings = convert_big_to_small_endian(header->size_dt_strings);
    size_dt_struct = convert_big_to_small_endian(header->size_dt_struct);

    uart_puts("=====================\r\n");
    uart_puts("Device Tree Address \t");
    uart_puts(itoa(*dt_addr, 16));
    uart_puts("\r\n");
    uart_puts("Total Size \t\t");
    uart_puts(itoa(totalsize, 16));
    uart_puts("\r\n");
    uart_puts("off_dt_struct \t\t");
    uart_puts(itoa(off_dt_struct, 16));
    uart_puts("\r\n");
    uart_puts("off_dt_strings \t\t");
    uart_puts(itoa(off_dt_strings, 16));
    uart_puts("\r\n");
    uart_puts("size_dt_struct \t\t");
    uart_puts(itoa(size_dt_struct, 16));
    uart_puts("\r\n");
    uart_puts("size_dt_strings \t");
    uart_puts(itoa(size_dt_strings, 16));
    uart_puts("\r\n");
    uart_puts("=====================\r\n");
}

void parse_dt(){
    uint64_t addr = DT_ADDR;
    uint64_t *dt_addr = (uint64_t*)addr;
    struct fdt_header* header = (struct fdt_header*)*dt_addr;

    uint32_t off_dt_struct, off_dt_strings, size_dt_struct;
    // totalsize = convert_big_to_small_endian(header->totalsize);
    off_dt_struct = convert_big_to_small_endian(header->off_dt_struct);
    off_dt_strings = convert_big_to_small_endian(header->off_dt_strings);
    // size_dt_strings = convert_big_to_small_endian(header->size_dt_strings);
    size_dt_struct = convert_big_to_small_endian(header->size_dt_struct);
    //////////////////////////////////////////

    uint64_t struct_addr = (uint64_t)*dt_addr + off_dt_struct;
    uint64_t string_addr = (uint64_t)*dt_addr + off_dt_strings;
    uint64_t struct_end_addr = struct_addr + size_dt_struct;
    uint32_t *struct_ptr = (uint32_t*)struct_addr;
    uint32_t cur_val, cur_len, cur_nameoff;
    char *cur_name, *cur_name_val;

    while((uint64_t)struct_ptr < struct_end_addr){
        cur_val = convert_big_to_small_endian(*struct_ptr);
        struct_ptr += 1;

        if(cur_val == FDT_BEGIN_NODE){
            cur_name = (char*)struct_ptr;
            uart_puts("============\r\n");
            uart_puts(cur_name);
            uart_puts("\r\n");
            cur_len = strlen(cur_name);
            struct_ptr += cur_len/4;
            struct_ptr += (cur_len%4 ? 1 : 0);
        }
        else if(cur_val == FDT_END_NODE){
            uart_puts("============\r\n");
        }
        else if(cur_val == FDT_PROP){
            cur_len = convert_big_to_small_endian(*struct_ptr);
            struct_ptr += 1;
            cur_nameoff = convert_big_to_small_endian(*struct_ptr);
            struct_ptr += 1;
        
            if(cur_len != 0){   // Not Empty Property
                cur_name = (char*)string_addr+cur_nameoff;
                cur_name_val = (char*)struct_ptr;
                struct_ptr += cur_len/4;
                struct_ptr += (cur_len%4 ? 1 : 0);
                uart_puts("Len: ");
                uart_puts(itoa(cur_len, 10));
                uart_puts("\r\n");
                uart_puts("Cur Name: ");
                uart_puts(cur_name);
                uart_puts("\r\n");
                uart_puts("Cur Value: ");
                uart_puts(cur_name_val);
                uart_puts("\r\n----------\r\n");
            }
        }
        else if(cur_val == FDT_NOP){
            ;
        }
        else if(cur_val == FDT_END){
            break;
        }
        else if(cur_val == PADDING){
            ;
        }
        else{
            uart_puts("ERROR! Unknown Node Value!\r\n");
            uart_puts(itoa(cur_val, 10));
            uart_puts("\r\n");
            return;
        }
    }
}