#include "kernel.h"
#include "uart.h"
#include "util.h"

extern char _end[];
extern char __kernel_begin[];
extern char __kernel_size[];
unsigned long int backup_address = 0x60000;

void load_new_kernel() 
{
	uart_putstr("Start Loading kernel image...\n");
	
	// 1. define new kernel address
	long long address_int = 0x80000;
	
	// 2. send kernel (use sendimg.py)
    uart_putstr("Please send kernel image from UART now...\n");
	
	// 3. read kernel size
	char k_size[20] = {0};
	uart_read_cmd(k_size);
    int kernel_size = atoi(k_size);
	
	// 4. show kernel size and load address
    uart_putstr("Kernel size: ");
    uart_putstr(k_size);
    uart_putstr("\t");
    uart_putstr("Load address: 0x80000");
    uart_putstr("\n");
	
	// 5. read new kernel form uart, start on new_address(0x80000) 
    uart_putstr("Starting to load target kernel\n");
	
	char* new_address = (char*)address_int;
    for (int i = 0; i < kernel_size; i++) 
	{
        char c = uart_getchar(); // all char should be get 
        new_address[i] = c;
    }
	
    uart_putstr("Finished load target kernel and running.\n");
	
	// 6. load new kernel
    void (*new_kernel)(void) = (void *)new_address;
    new_kernel();
}

void backup_old_kernel()
{
    char *kernel = __kernel_begin;
	unsigned long int size = (unsigned long int)__kernel_size;
    char *backup = (char *)(backup_address);
	
    uart_putstr("Starting to backup bootloader\n");
	
	// move bootloader to backup_address(0x60000)
    while (size--) 
	{
        *backup = *kernel;
        kernel++;
        backup++;
    }
	
    uart_putstr("Finished backup bootloader\n");
}

void load_image()
{
	// 1. backup bootloader kernel to 0x60000
	backup_old_kernel();
	
	// 2. calculate new 'load_new_kernel' funciton address

	// because old kernel are backup to backup_address
	// so load_new_kernel function need move to new address 
	// offest = load_target_func_address - kernel_start
	
    void (*load_target_ptr)() = load_new_kernel;
    unsigned long int load_target_func_address = (unsigned long int) load_target_ptr;
    void (*load_kernel)() = 
        (void (*)())( backup_address + (load_target_func_address - (unsigned long int)__kernel_begin) );
	
	// 3. load new kernel form uart
    load_kernel();
}
