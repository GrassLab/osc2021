#include "cpio.h"

unsigned long align(unsigned long  reminder, unsigned long  base){
    return (base - (reminder % base)) % base; 
}


void extract_header(struct cpio_newc_header *cpio_addr, struct cpio_info *size_info){
    size_info->file_size = hextoint(cpio_addr->c_filesize, 8);
    size_info->file_align = align(size_info->file_size, 4);
    size_info->name_size = hextoint(cpio_addr->c_namesize, 8);
    size_info->name_align = align(size_info->name_size + CPIO_SIZE, 4);
    size_info->offset = CPIO_SIZE + size_info->file_size + size_info->file_align + size_info->name_size + size_info->name_align;
}



void cpio_list(){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        uart_puts(pathname);
        uart_puts("\r\n");
        now_ptr += size_info.offset;//next_addr_offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
           
    }
}

void cpio_cat(char *args){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
    int flag = 0;
    while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(args, pathname) == 0){
			uart_puts("CPIO address :");
			uart_put_hex((unsigned long)cpio_addr);
			uart_puts("\r\n\r\n");
            uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            uart_puts("\r\n");
            flag = 1;
        }
        now_ptr += size_info.offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
    }
    if(flag == 0){
        uart_puts("No such file: ");
        uart_puts(args);
        uart_puts("\r\n");
    }
}

char* find_app_addr(char* target){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
	while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(target, pathname) == 0){
			//uart_puts("CPIO address :");
			//uart_put_hex((unsigned long)cpio_addr);
			//uart_puts("\r\n\r\n");
            //uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            //uart_puts("\r\n");
            return (char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align);
        }
        now_ptr += size_info.offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
	}
	return NULL;
}

int find_app_size(char* target){
    char *now_ptr = CPIO_ADDR;
    struct cpio_newc_header *cpio_addr = (struct cpio_newc_header* )now_ptr;
    struct cpio_info size_info;
	while(1){
        extract_header(cpio_addr, &size_info);
        char *pathname = (char*)((char*)cpio_addr + CPIO_SIZE);
        if(strcmp("TRAILER!!!", pathname) == 0) break;
        if(strcmp(target, pathname) == 0){
			//uart_puts("CPIO address :");
			//uart_put_hex((unsigned long)cpio_addr);
			//uart_puts("\r\n\r\n");
            //uart_puts_bySize((char*)((char*)cpio_addr + CPIO_SIZE + size_info.name_size + size_info.name_align), size_info.file_size);
            //uart_puts("\r\n");
            return size_info.file_size;
        }
        now_ptr += size_info.offset;
        cpio_addr = (struct cpio_newc_header* )now_ptr;
	}
	return 0;
}

void load_app(char *args){
	char *new_addr_ptr = NEW_ADDR;
	char *new_sp_ptr = NEW_SP;
	char* app_addr = find_app_addr(args);
	if(!app_addr){
		uart_puts("App not found!\n");
		return;
	}
	int app_size = find_app_size(args);
	uart_puts("APP address : ");
	uart_put_hex((unsigned long)app_addr);
	uart_puts("\r\n");
	uart_puts("APP size : ");
	uart_put_int(app_size);
	uart_puts("\r\n");
	//uart_puts_bySize((char*)app_addr, find_app_size(args));
	char* target=NEW_ADDR;
	while(app_size--){
		*target=*app_addr;
		target++;
		app_addr++;
	}
	//uart_puts_bySize(NEW_ADDR, find_app_size(args));
	
	uart_puts("loading app...\n");
	unsigned long target_addr = (unsigned long)new_addr_ptr;
	unsigned long target_sp = (unsigned long)new_sp_ptr;
	core_timer_enable();
	//change exception level
	//asm volatile("mov x0, 0x3c0			\n");//disable interrupt
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(target_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(target_sp));
	
	//enable the core timer’s interrupt
	//asm volatile("mov x0, 1				\n");
	//asm volatile("msr cntp_ctl_el0, x0	\n");
	//asm volatile("mrs x0, cntfrq_el0	\n");
	//asm volatile("add x0, x0, x0		\n");
	//asm volatile("msr cntp_tval_el0, x0	\n");
	//asm volatile("mov x0, 2				\n");
	//asm volatile("ldr x1, =0x40000040	\n");//#define CORE0_TIMER_IRQ_CTRL 0x40000040
	//asm volatile("str w0, [x1]			\n");
	

	asm volatile("eret					\n");


}




