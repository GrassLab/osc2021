#include "bootloader.h"
extern char __start_addr[];
extern char __end[];


void bootloader_loadimg(){
    uart_puts("*** Copy bootloader ...\r\n");
    copy_bootloader();
    uart_puts("*** Copy bootloader done.\r\n");
    uart_puts("*** You can send new kernel through uart now.\r\n");
    void (*func_ptr)() = load_new_kernel;
    unsigned long int func_addr = (unsigned long int)func_ptr;
    void (*function_call)(void) = (void (*)(void))(func_addr - (unsigned long int)__start_addr + TMP_ADDR);
    function_call();
    //load_new_kernel();
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
    // uart_puts("copy new kernel to 0x80000\r\n");
    char *current_addr;
    current_addr = KERNEL_ADDR;
    char c;
    uart_puts("*** New kernel size: ");
    int img_size = uart_get_int();
    uart_puts("\r\n");
    for(int i = 0; i < img_size; ++i){
        c = uart_get();
        *(current_addr + i) = c;
    }
    uart_puts("*** Transmit new kernel done.\r\n");
    uart_puts("*** Press enter to start with new kernel\r\n");
    void (*new_kernel_start)(void) = (void*)KERNEL_ADDR;
    new_kernel_start();
}