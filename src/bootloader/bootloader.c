#include "mini_uart.h"
#include "string.h"

void* bootloader(void *kernel_entry)
{
    init_uart();
    
    char kernel_code = getchar();
    puts("transmitting...\r\n");
    
    // protocol s = start, end = end
    if (kernel_code == 's') {
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
            
            // putchar(kernel_code);

            if (buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd') {
                kernel_tmp -= 2;
                memset(kernel_tmp, 0, 3);
                break;
            }

            kernel_tmp++;
        }
    }

    puts("finished\r\n\n");
    
    return kernel_entry;
}