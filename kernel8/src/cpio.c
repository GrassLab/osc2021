#include "cpio.h"


unsigned long  padding(unsigned long  size, unsigned long  multiplier){
    if(multiplier <= 0) return 0;
    else return (multiplier - (size % multiplier)) % multiplier;
}


void extract_header(struct cpio_header *cpio_addr, struct cpio_info *size_info){
    size_info->file_size = hextoint(cpio_addr->c_filesize, 8);
    size_info->file_padding = padding(size_info->file_size, 4);
    size_info->name_size = hextoint(cpio_addr->c_namesize, 8);
    size_info->name_padding = padding(size_info->name_size + 110, 4);
    size_info->offset = 110 + size_info->file_size + size_info->file_padding + size_info->name_size + size_info->name_padding;;
}

void cpio_list(){
    char *now_ptr = CPIO_ADDR;
    struct cpio_header *cpio_addr = (struct cpio_header* )now_ptr;
    struct cpio_info size_info;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + 110);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        uart_puts(pathname);
        uart_puts("\r\n");
        now_ptr += size_info.offset;//next_addr_offset;
        cpio_addr = (struct cpio_header* )now_ptr;
           
    }
}

void cpio_cat(char *args){
    char *now_ptr = CPIO_ADDR;
    struct cpio_header *cpio_addr = (struct cpio_header* )now_ptr;
    struct cpio_info size_info;
    int flag = 0;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + 110);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(args, pathname) == 0){
            uart_puts_bySize((char*)((char*)cpio_addr + 110 + size_info.name_size + size_info.name_padding), size_info.file_size);
            uart_puts("\r\n");
            flag = 1;
        }
        now_ptr += size_info.offset;
        cpio_addr = (struct cpio_header* )now_ptr;
    }
    if(flag == 0){
        uart_puts("No such file: ");
        uart_puts(args);
        uart_puts("\r\n");
    }
}



