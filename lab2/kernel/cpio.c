#include "cpio.h"
#include "uart.h"
#include "util.h"

//https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5
//The pathname is followed by NUL bytes so that the total size of the fixed
//     header plus pathname is a multiple of four. Likewise, the file data is
//     padded to a multiple of four bytes. Note that this format supports only
//     4 gigabyte files (unlike the older	ASCII format, which supports 8 giga-
//     byte files).
unsigned long long align_to_4(unsigned long long size) 
{
	unsigned long long residual = size % 4;
	
	if(residual > 0)
		return size + (4 - residual);
	else
		return size;
}

// list rootfs file
void cpio_list() 
{
	unsigned long long ptr = 0x8000000;
	struct cpio_newc_header *header;
	
	while (1) 
	{
		header = (struct cpio_newc_header *)ptr;
		if (strcmpn(header->c_magic, CPIO_MAGIC, 6))
			return;

		char* fileName = (char*)(ptr + sizeof(struct cpio_newc_header));
		if (strcmpn(fileName, CPIO_TRAILER, 10))
			return;
		
		unsigned long namesize = hex2int(header->c_namesize, 8);
		unsigned long filesize = hex2int(header->c_filesize, 8);
		
		if(namesize > 0)
		{
			uart_putstr(fileName);
			uart_putstr("  ");
		}
		
		ptr = (unsigned long long)fileName;
		ptr = align_to_4(ptr + namesize);
		ptr = align_to_4(ptr + filesize);
	}
}

// get file content
char* cpio_content(char* name)
{
	unsigned long long ptr = 0x8000000;
	struct cpio_newc_header *header;
 
	while (1)
	{
		header = (struct cpio_newc_header *)ptr;
		if (strcmpn(header->c_magic, CPIO_MAGIC, 6)) 
			return 0;
		
		char* fileName = (char*)(ptr + sizeof(struct cpio_newc_header));	
		if (strcmpn(fileName, CPIO_TRAILER, 10))
			return 0;

		unsigned long namesize = hex2int(header->c_namesize, 8);
		unsigned long filesize = hex2int(header->c_filesize, 8);

		if (strcmp(fileName, name)) 
		{
			if (filesize > 0) 
			{
				ptr = (unsigned long long)fileName;
				ptr = align_to_4(ptr + namesize);
				return (char*)ptr;
			}

			return 0;
		} 
		else 
		{
			ptr = (unsigned long long)fileName;
			ptr = align_to_4(ptr + namesize);
			ptr = align_to_4(ptr + filesize);
		}
	}		
}