#include "mini_uart.h"
#include "string.h"
#include "bootloader.h"

void* bootloader(void *kernel_entry)
{
    init_uart();
    puts("waiting for kernel transmission\r\n");
    char kernel_code;
    while(1) {
        if (getchar() == 's' && getchar() == 't' && getchar() == 'a' && getchar() == 'r' && getchar() == 't') {
            puts("transmitting...\r\n");

            char *kernel_tmp = kernel_entry;

            char buffer[3];
            memset(buffer, 0, sizeof(char) * 3);
            int counter = 0;
            while(1) {
                kernel_code = getchar();

                if (counter < 3) {
                    buffer[counter] = kernel_code;
                    counter++;
                } else {
                    buffer[0] = buffer[1];
                    buffer[1] = buffer[2];
                    buffer[2] = kernel_code;
                }

                *kernel_tmp = kernel_code;

                if (buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd') {
                    kernel_tmp -= 2;
                    memset(kernel_tmp, 0, 3);
                    break;
                }

                kernel_tmp++;
            }

            break;
        }
    }

    puts("finished\r\n\n");
    
    return kernel_entry;
}

void do_relocate(void *target)
{
    char *pAddr = (void *)BOOTLOADER_ORIGNINAL_ADDRESS;
    char *targetTmp = (char *)target;
    
    while (pAddr != (void *)BOOTLOADER_ORIGINAL_STACK_TOP)
    {
        *targetTmp = *pAddr;

        targetTmp++;
        pAddr++;
    }

    return;
}