#include "cpio.h"
#include "uart.h"
#include "util.h"
#include "timer.h"

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
	//unsigned long int ptr = 0x20000000;
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
	//unsigned long int ptr = 0x20000000;
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

// required 1-2 Add a command that can load a user program in the initramfs. Then, use eret to jump to the start address.
void cpio_run_user_program(char* name, int enable_timer)
{
	unsigned long long ptr = 0x8000000;  // cpio address
	unsigned long long loadAddr = 0x4000000;
	
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
		
		if (strcmp(fileName, name)) 
		{
			char buf[16] = {0};
			unsignedlonglongToStr(filesize, buf);
			uart_putstr(buf);
			uart_putstr("\n");
			
			if (filesize > 0) 
			{				
				ptr = (unsigned long long)fileName;
				ptr = align_to_4(ptr + namesize);
			
				uart_putstr("start load user program...\n");
			
				// load user program
				char* target = (char*)loadAddr;
				char* cpio = (char*)ptr;
				for (int i = 0; i < filesize; i++)
				{
					*target = *cpio;
					target++;
					cpio++;
				}
				
				uart_putstr("complete...\n");
				
				// required 1-2, EL1 to EL0
				// 1. load a user program in the initramfs to a specific address
				// 2. set spsr_el1 to 0x3c0 and elr_el1 to the program’s start address.    
				// 3. set the user program’s stack pointer to a proper position by setting sp_el0.
				// 4. issue eret to return to the user code.
				
				if(enable_timer)
				{
					core_timer_enable();
					asm volatile("mov x0, 0x0			\n\t"); // required 2, enable interrupt
				}
				else
				{
					asm volatile("mov x0, 0x3c0			\n\t"); // required 1-2, disable interrupt
				}
				
				asm volatile("msr spsr_el1, x0		\n\t");
				asm volatile("msr elr_el1, %0		\n\t"::"r"(loadAddr));
				asm volatile("msr sp_el0, %0		\n\t"::"r"(loadAddr));
				asm volatile("eret					\n");
				
				break;
			}
		} 
		else 
		{
			ptr = (unsigned long long)fileName;
			ptr = align_to_4(ptr + namesize);
			ptr = align_to_4(ptr + filesize);
		}
	}		
}

int cpio_load_user_program_and_get_size(char* name, unsigned long long loadAddr)
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
				
				// load user program
				char* target = (char*)loadAddr;
				char* user_progame = (char*)ptr;
				for (int i = 0; i < filesize; i++)
				{
					*target = *user_progame;
					target++;
					user_progame++;
				}
				
				return filesize;
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