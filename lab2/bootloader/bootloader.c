#include "uart.h"
#include "bootloader.h"

#define OLD_BOOTLOADER_ADDR 0x80000;
#define NEW_BOOTLOADER_ADDR 0x60000;

extern unsigned long __program_size;

void relocate(char *old_addr, char *new_addr)
{
    uart_put_str("start to relocate.\n");
    unsigned long program_size = (unsigned long) &__program_size;
    for(int i = 0; i < program_size; i++)
    {
        *new_addr++ = *old_addr++;
    }
    
    long offset = (long) new_addr - (long) old_addr;
    
    asm volatile(
        "ldr x10, [sp, #8]\n\t"
        "add x10, x10 , %0\n\t"
        "str x10, [sp, #8]\n\t"
        :
        :"r" (offset)
    );
    
}

void bootloader()
{
    char *old_addr = (char *) OLD_BOOTLOADER_ADDR;
    char *new_addr = (char *) NEW_BOOTLOADER_ADDR;
    relocate(old_addr, new_addr);
    uart_put_str("already jump to new location.\n");

    unsigned int kernel_size;
    uart_put_str("wait kernel size...\n");
    kernel_size = uart_get_int();
    uart_put_str("kernel size: ");
    uart_put_int(kernel_size);
    uart_put_str("\n");
    
    
    for(int i = 0; i < kernel_size; i++)
    {
        char *kernel_address = (char *) OLD_BOOTLOADER_ADDR;
        char c = uart_get_char();
        kernel_address[i] = c;
    }
    uart_put_str("kernel received.\n");

    uart_put_str("kernel start.\n");
    for(int i = 0; i < 115200; i++)
        asm volatile("nop");
    
    char *kernel_start_addr = (char *) OLD_BOOTLOADER_ADDR;
    asm volatile(
        "add x10, %0, #0\n\t"
        "br x10" 
        :
        :"r" (kernel_start_addr)
    );
    

}