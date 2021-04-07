#include "cpio.h"

unsigned long align(unsigned long  reminder, unsigned long  base){
    return (base - (reminder % base)) % base; 
}


void extract_header(struct cpio_newc_header *cpio_addr, struct cpio_info *size_info){
    size_info->file_size = hextoint(cpio_addr->c_filesize, 8);
    size_info->file_align = align(size_info->file_size, 4);
    size_info->name_size = hextoint(cpio_addr->c_namesize, 8);
    size_info->name_align = align(size_info->name_size + CPIO_SIZE, 4);
    size_info->offset = CPIO_SIZE + size_info->file_size + size_info->file_align + size_info->name_size + size_info->name_align;
}

void cpio_list(){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        uart_puts(pathname);
        uart_puts("\r\n");
        now_ptr += size_info.offset;//next_addr_offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
           
    }
}

void cpio_cat(char *args){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
    int flag = 0;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(args, pathname) == 0){
            uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            uart_puts("\r\n");
            flag = 1;
        }
        now_ptr += size_info.offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
    }
    if(flag == 0){
        uart_puts("No such file: ");
        uart_puts(args);
        uart_puts("\r\n");
    }
}