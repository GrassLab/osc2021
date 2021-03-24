#include "uart.h"
#include "cpio.h"
#include "string.h"

#define CPIO_ADDR   0x8000000
#define CPIO_HEADER_SIZE 110

unsigned int align4(unsigned int num)
{
    if (num == 0)
        return 0;
    return (((num - 1) / 4) + 1) * 4;
}

void list()
{
    int namesize, filesize;
    cpio_newc_header *cpio_head = (cpio_newc_header *)CPIO_ADDR;
    
    while(!strlcmp(cpio_head->c_mode, "00000000", 8))
    {
        namesize = str2int(cpio_head->c_namesize, 8);
        filesize = str2int(cpio_head->c_filesize, 8);
        if(strlcmp(cpio_head->c_mode, "000081B4", 8))
        {
            uart_put_str((char *)cpio_head + CPIO_HEADER_SIZE);
            uart_put_str("\n");
        }
        cpio_head = (cpio_newc_header *)((char *)(cpio_head) + align4(CPIO_HEADER_SIZE + namesize) + align4(filesize));
    }
}

void cat(char *pathname)
{
    int namesize, filesize;
    cpio_newc_header *cpio_head = (cpio_newc_header *)CPIO_ADDR;
    
    while(!strlcmp(cpio_head->c_mode, "00000000", 8))
    {
        namesize = str2int(cpio_head->c_namesize, 8);
        filesize = str2int(cpio_head->c_filesize, 8);
        
        if(strlcmp((char *)cpio_head + CPIO_HEADER_SIZE, pathname, strlen(pathname)))
        {
            char *file_content = (char *)cpio_head + align4(CPIO_HEADER_SIZE + namesize);
            for(int i = 0; i < filesize ; i++)
            {
                uart_send(file_content[i]);
            }
            uart_send('\n');
            return;
        }
        cpio_head = (cpio_newc_header *)((char *)(cpio_head) + align4(CPIO_HEADER_SIZE + namesize) + align4(filesize));
    }
    uart_put_str("The file is not found.\n");
}