#include "mini_uart.h"

void* bootloader(void *kernel_entry)
{
    init_uart();
    
    char kernel_code = getchar();
    
    // protocol s = start, e = end
    if (kernel_code == 's') {
        char *kernel_tmp = kernel_entry;

        while((kernel_code = getchar()) != 'e') {
            *kernel_tmp = kernel_code;
            kernel_tmp++;
        }
    }


    return kernel_entry;
}