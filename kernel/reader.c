#include "reader.h"
#include "process.h"
#include "allocator.h"
#include "../lib/uart.h"
#include "../lib/string.h"

const int FILE_NUM_LIMITED = 100;
char * address_cpio = (char*)0x2000;
char * address_user_program = (char*)0x3000;
char * address_user_stack = (char*)0x3000;
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
    LoadUserProgram(arg);

    FromEl1toEl0();
    EnableTimer();
    EnableInterrupt();

    asm volatile("eret"); // return user code
}

void LoadUserProgram(char filename[])
{
    int file_i = 0;

    for (int i = 0; i < FILE_NUM_LIMITED; ++i)
    {
        if (!strcmp(cpio_archive[i].filename, "TRAILER!!!"))
	    {
	        uart_puts("File: ");
	        uart_puts(filename);
	        uart_puts(" is not found.\n");
	        return;
	    }
	    else if (!strcmp(cpio_archive[i].filename, filename))
	    {
	        file_i = i;

	        break;
        }
    }

    address_user_program = buddy_alloc(PROCESS_SIZE);
    address_user_stack = address_user_program + PROCESS_SIZE;
    for (int i = 0; i < cpio_archive[file_i].size; ++i)
    {
        *(address_user_program + i) = cpio_archive[file_i].contents[i];
    }
}

void FromEl1toEl0()
{
    asm volatile("mov x0,       0x3c0"); // save current processor's state(PSTATE) in 'SPSR_ELx'
    asm volatile("msr spsr_el1, x0   ");
    asm volatile("msr elr_el1,  %0   "::"r"(address_user_program)); // save exception return address
    asm volatile("msr sp_el0,   %0   "::"r"(address_user_stack)); // set user program's stack pointer to proper position by setting 'sp_el0'
}

void EnableTimer()
{
    asm volatile("mov x0,             1");
    asm volatile("msr cntp_ctl_el0,  x0");
    asm volatile("mrs x0,    cntfrq_el0");
    asm volatile("add x0, x0, x0       ");
    asm volatile("msr cntp_tval_el0, x0");
    asm volatile("mov x0,             2");
    asm volatile("ldr x1,   =0x40000040");
    asm volatile("str w0,          [x1]");
}

void EnableInterrupt()
{
    asm volatile("mov x0,        0x0");
    asm volatile("msr spsr_el1,   x0");
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

void LoadArgument(char * argv[])
{
    int argc = 0;
    while(argv[argc] != 0)
    {
        argc++;
    }

    address_user_stack -= (1 + 1 + argc + 1 + argc) * 8;    // argc + argv** + argv* + NULL + argv
    address_user_stack -= (char*)((int)address_user_stack & 15);
    
    char * temp_stack_ptr = address_user_stack;
    *(temp_stack_ptr) = argc;
    temp_stack_ptr += 8;
    *(unsigned long *)(temp_stack_ptr) = (unsigned long)(temp_stack_ptr + 8);    // argv** = argv[0]
    temp_stack_ptr += 8;

    char * argv_addr = temp_stack_ptr + argc * 8;   // argv*
    *argv_addr = NULL;
    argv_addr += 8;
    for (int i = 0; i < argc; ++i)
    {
        *(unsigned long*)(temp_stack_ptr) = (unsigned long)(argv_addr + 8 * i);  // argv[0] = argv[0][0]
        *(argv_addr + 8 * i) = argv[i];
    }
    
    uart_puts_h(address_user_stack);
    uart_puts(" ");
    uart_puts_i(*address_user_stack);

    uart_puts("\n");

    for (int i = 0; i < 10; ++i)
    {
        uart_puts_h(*(unsigned long*)(address_user_stack + i * 8));
        uart_puts(" ");
    }
    uart_puts("\n");
    uart_puts("\n");
}

void cpio_exec(char * filename, char * argv[])
{
    ReadCpio();
    LoadUserProgram(filename);
    LoadArgument(argv);

    FromEl1toEl0();
    //EnableTimer();
    EnableInterrupt();
    
    asm volatile("eret"); // return user code
}