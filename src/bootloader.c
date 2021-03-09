#include "bootloader.h"
extern char __start_addr[];
extern char __end[];
#define KERNEL_ADDR ((char)(0x80000))
#define TMP_ADDR ((char)(0x100000))

void bootloader_loadimg(){
    copy_bootloader();
    uart_puts("Copy bootloader done.\r\n");
    uart_puts("Send new kernel through uart\r\n");
    void (*func_ptr)() = load_new_kernel;
    unsigned long int func_addr = (unsigned long int)func_ptr;
    void (*function_call)(void) = (void (*)(void))(func_addr - (unsigned long int)__start_addr + TMP_ADDR);
    function_call();
}
void copy_bootloader(){
    // move self(bootloader) to TMP_ADDR
    char *current_addr = __start_addr;
    char *end_addr = __end;
    char *target_addr = TMP_ADDR;
    while(current_addr <= end_addr){
        *target_addr = *current_addr;
        target_addr++;
        current_addr++;
    }
}
void load_new_kernel(){
    char *current_addr = KERNEL_ADDR;
    char *end = KERNEL_ADDR;
    char c;
    int img_size = uart_get_int();
    for(int i = 0; i < img_size; ++i){
        c = uart_get();
        *(current_addr + i) = c;
        end++;
    }
    uart_puts("transmit new kernel done.\r\n");
    void (*new_kernel_start)(void) = KERNEL_ADDR;
    new_kernel_start();
}