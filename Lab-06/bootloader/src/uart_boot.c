#include "uart_boot.h"

extern char __start[];
extern char __end[];


void loadimg(){
	uart_puts("Copy bootloader to 0x60000\r\n");
    char *current_addr = __start;
    char *end_addr = __end;
    char *target_addr = TEMP_ADDR;
    while(current_addr <= end_addr){
        *target_addr = *current_addr;
        target_addr++;
        current_addr++;
    }
	uart_puts("Copy bootloader finish\r\n");
    void (*func_ptr)() = load_new_kernel;
    unsigned long int func_addr = (unsigned long int)func_ptr;
    void (*function_call)(void) = (void (*)(void))(func_addr - (unsigned long int)__start + TEMP_ADDR);
    function_call();

}

void load_new_kernel(){
    uart_puts("Copy new kernel to 0x80000\r\n");
    char *current_addr;
    current_addr = KERNEL_ADDR;
    char c;
    uart_puts("New kernel size: ");
    int img_size = uart_get_int();
    uart_puts("\r\n");
    for(int i = 0; i < img_size; ++i){
        c = uart_get();
        *(current_addr + i) = c;
    }
    uart_puts("Transmit done.\r\n                        ");
    void (*new_kernel_start)(void) = (void*)KERNEL_ADDR;
    new_kernel_start();
}

