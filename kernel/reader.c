#include "reader.h"
#include "uart.h"
#include "string.h"

const int FILE_NUM_LIMITED = 100;
char * address_cpio = (char*)0x2000;
char * address_user_program = (char*)0x3000;
struct cpio_file cpio_archive[100];

void Cpiols()
{
    ReadCpio();
    PrintCpio();
}

void Cpiocat(char arg[])
{
    ReadCpio();
    PrintFileContent(arg);
}

void Cpioexe(char arg[])
{
    ReadCpio();
    int file_i = 0;

    for (int i = 0; i < FILE_NUM_LIMITED; ++i)
    {
        if (!strcmp(cpio_archive[i].filename, "TRAILER!!!"))
	    {
	        uart_puts("File: ");
	        uart_puts(arg);
	        uart_puts(" is not found.\n");
	        return;
	    }
	    else if (!strcmp(cpio_archive[i].filename, arg))
	    {
	        file_i = i;

	        break;
        }
    }

    for (int i = 0; i < cpio_archive[file_i].size; ++i)
    {
        *(address_user_program + i) = cpio_archive[file_i].contents[i];
    }

    // from el1 to el0
    asm volatile("mov x0,       0x3c0\n"); // save current processor's state(PSTATE) in 'SPSR_ELx'
    asm volatile("msr spsr_el1, x0   \n");
    asm volatile("msr elr_el1,  %0   \n"::"r"(address_user_program)); // save exception return address
    asm volatile("msr sp_el0,   %0   \n"::"r"(address_user_program)); // set user program's stack pointer to proper position by setting 'sp_el0'
    
    asm volatile("eret"); // return user code
}

void ReadBytesData(char data[], int offset, int bytes)
{
    for (int i = 0; i < bytes; ++i)
    {
       data[i] = *(address_cpio + offset + i);
    }
}

void ReadCpio()
{
    int isEnd = 0;
    int count = 0;
    int offset = 0;

    while (!isEnd)
    {
        ReadCpioHeader(&cpio_archive[count], offset);
	    isEnd = ReadCpioContent(&cpio_archive[count], offset);

	    offset += cpio_archive[count].size;
	
	    ++count;
    }
}

void PrintCpio()
{
    for (int i = 0; i < FILE_NUM_LIMITED; ++i)
    {
        if (!strcmp(cpio_archive[i].filename, "TRAILER!!!"))
        {
            uart_puts("\n");
            break;
        }
        if (!strcmp(cpio_archive[i].filename, "."))
        {
            continue;
        }
        else
        {
            uart_puts("\t");
            uart_puts(cpio_archive[i].filename);
        }
    }
}

void PrintFileContent(char arg[])
{
    for (int i = 0; i < FILE_NUM_LIMITED; ++i)
    {
        if (!strcmp(cpio_archive[i].filename, "TRAILER!!!"))
	    {
	        uart_puts("File: ");
	        uart_puts(arg);
	        uart_puts(" is not found.\n");
	        break;
	    }
	    else if (!strcmp(cpio_archive[i].filename, arg))
	    {
	        uart_puts(cpio_archive[i].contents);
	        break;
        }
    }
}

void ReadCpioHeader(struct cpio_file * cpio_, int offset)
{
    ReadBytesData(cpio_->cpio_header.c_magic, offset, 6);
    offset += 6;

    ReadBytesData(cpio_->cpio_header.c_ino, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_mode, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_uid, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_gid, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_nlink, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_mtime, offset, 8);
    offset += 8;   
    
    ReadBytesData(cpio_->cpio_header.c_filesize, offset, 8);
    offset += 8;
   
    ReadBytesData(cpio_->cpio_header.c_devmajor, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_devminor, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_rdevmajor, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_rdevminor, offset, 8);
    offset += 8;
   
    ReadBytesData(cpio_->cpio_header.c_namesize, offset, 8);
    offset += 8;
    
    ReadBytesData(cpio_->cpio_header.c_check, offset, 8); 
    
    cpio_->size = 110;
}

int Hex2Int(char byte[])
{
    int res = 0;

    for(int i = 0; i < 8; ++i)
    {
        if (byte[i] >= '0' && byte[i] <= '9') byte[i] -= '0';
	else if (byte[i] >= 'A' && byte[i] <= 'F') byte[i] = byte[i] - 'A' + 10;

        res |= byte[i] << (8 - i - 1) * 4;
    }

    return res;
}

int ReadCpioContent(struct cpio_file * cpio_, int offset)
{
    int isEnd = 0;
    int namesize = Hex2Int(cpio_->cpio_header.c_namesize);
    int filesize = Hex2Int(cpio_->cpio_header.c_filesize);

    for (int i = 0; i < namesize - 1; ++i)
    {
        cpio_->filename[i] = *(address_cpio + offset + cpio_->size + i);
    }
    cpio_->filename[namesize - 1] = '\0';
    cpio_->size = (cpio_->size + namesize + 3) & (~3);

    for (int i = 0; i < filesize; ++i)
    {
        cpio_->contents[i] = *(address_cpio + offset + cpio_->size + i);
    }
    cpio_->contents[filesize] = '\0';
    cpio_->size = (cpio_->size + filesize + 3 ) & (~3);

    if (!strcmp(cpio_->filename, "TRAILER!!!"))
    {
        isEnd = 1;
    }

    return isEnd;
}
