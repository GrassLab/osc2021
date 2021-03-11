#include "reader.h"
#include "uart.h"

char* address_cpio = (char*)0x2000;

struct cpio_newc_header {
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
};

struct cpio_newc_header cpio_header;

void ReadCpio()
{
    uart_puts("Start to read cpio files...\n");

    ReadCpioHeader();
}

void ReadCpioHeader()
{
    for (int i = 0; i < 10 ; ++i)
    {
        uart_send(*(address_cpio + i));
    }
}
