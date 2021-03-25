#include "system.h"
#include "string.h"
#include "uart.h"
#include "bootloader.h"
#include "dtb_parser.h"

struct cmd{
    char input[20];
    char description[30];
    void(*callback)(char*);
    
};
struct cmd cmd_list[SYS_CMD_NUM] = {
    {.input = "help", .description="list commands", .callback = sys_help},
    {.input = "hello", .description="print hello world",.callback = sys_hello},
    {.input = "reboot", .description="reset raspi3",.callback = sys_reboot},
    {.input = "loadimg", .description="load new kernel",.callback = bootloader_loadimg},
    {.input = "ls", .description="list rootfs",.callback = sys_list},
    {.input = "cat", .description="show content of file", .callback = sys_cat},
    {.input = "dtb_init", .description="init device by calling driver", .callback = dtb_init}
};


uint32_t sys_get32bits(char* ptr){
    uint32_t res = 0;
    for(int i = 0; i < 4; ++i){
        res <<= 8;
        res += ptr[i];
    }
    return res;
}
uint64_t sys_get64bits(char* ptr){
    return ((uint64_t)sys_get32bits(ptr) << 32) + (uint64_t)(sys_get32bits(ptr + 4));
}
unsigned long long int hex2int(char* addr, const int size){
    unsigned long long int res = 0;
    char c;
    for(int i = 0 ;i < size; ++i){
        res <<= 4;
        c = *(addr + i);
        if(c >= '0' && c <= '9') res += c - '0';
        else if(c >= 'A' && c <= 'F') res += c - 'A' + 10;
        else if(c >= 'a' && c <= 'f') res += c - 'a' + 10;
    }

    return res;
}
unsigned long long int need_padding(unsigned long long int size, unsigned long long int multiplier){
    if(multiplier <= 0) return 0;
    else return (multiplier - (size % multiplier)) % multiplier;
}

void extract_header(struct cpio_newc_header *cpio_addr, struct cpio_size_info *size_info){
    size_info->file_size = hex2int(cpio_addr->c_filesize, 8);
    size_info->file_padding = need_padding(size_info->file_size, 4);
    size_info->name_size = hex2int(cpio_addr->c_namesize, 8);
    size_info->name_padding = need_padding(size_info->name_size + 110, 4);
    size_info->offset = 110 + size_info->file_size + size_info->file_padding + size_info->name_size + size_info->name_padding;;
}

void sys_list(char* args){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_size_info size_info;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + 110);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        uart_puts(pathname);
        uart_puts("\r\n");
        now_ptr += size_info.offset;//next_addr_offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
           
    }
}

void system_command(char* buf){
    char *args = buf;
    int arg_idx = 0;
    for(; buf[arg_idx] != '\0'; ++arg_idx, ++args){
        if(buf[arg_idx] == ' '){
            buf[arg_idx] = '\0';
            args = buf + arg_idx + 1;
            break;
        }
    }
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        if(strcmp(cmd_list[i].input, buf) == 0){
            cmd_list[i].callback(args);
            break;
        }
    }
    return;
}
void sys_cat(char* args){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_size_info size_info;
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
        cpio_addr = (struct cpio_newc_header* )now_ptr;
    }
    if(flag == 0){
        uart_puts("No such file: ");
        uart_puts(args);
        uart_puts("\r\n");
    }
}
void swap(int* a, int* b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
void* malloc(int size){
    memFrame* ptr = buddy_alloc(size);
    return (void*)(ptr->addr);
}
void sys_help(char* args){
    uart_puts("[Command] : [Description]\r\n");
    for(int i = 0; i < SYS_CMD_NUM; ++i){
        uart_puts(cmd_list[i].input);
        uart_puts(" : ");
        uart_puts(cmd_list[i].description);
        uart_puts("\r\n");
    }
    
}
void sys_hello(char* args){
    uart_puts("Hello World !\r\n");
}
void sys_reboot(char* args){
    reset(100);
    while(1);
}
void *__memset(void* buf, int c, int size){
    char *ptr = buf;
    while(size--)
        *ptr++ = c;
    return buf;
}
void reset(int tick){
    *PM_RSTC = PM_PASSWORD | 0x20;
    *PM_WDOG = PM_PASSWORD | tick;
}
void cancel_reset(){
    *PM_RSTC = PM_PASSWORD | 0x00;
    *PM_WDOG = PM_PASSWORD | 0x00;

}
