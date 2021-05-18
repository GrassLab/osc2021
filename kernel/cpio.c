#include "uart.h"
#include "utils.h"
#include "string.h"
#include "allocator.h"
#include "xcpt_func.h"
#include "cpio.h"

//volatile unsigned char *cpio_address_base = (unsigned char *) 0x20000000;
// on qemu
volatile unsigned char *cpio_address_base = (unsigned char *) 0x08000000;

/*
unsigned char *findEntry(char * target) {
    unsigned char *cpio_address = cpio_address_base;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = (char *)cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            uart_puts("No such file\n");
            break;
        }

        cpio_address += name_size;
        unsigned char *file_data = (unsigned char *)cpio_address;
        if(!strcmp(path_name, target)) {
            return cpio_address;
        }
        cpio_address += file_size;
    }
    return 0;
}
*/

void getFileData(char *target) {
    uart_puts("Please enter file name: ");
    uart_read_line(target, 1);
    uart_send('\r');
    volatile unsigned char *cpio_address = cpio_address_base;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = (char*)cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            uart_puts("No such file\n");
            break;
        }

        cpio_address += name_size;
        unsigned char *file_data = (unsigned char *)cpio_address;
        if(!strcmp(path_name, target)) {
            for(int i = 0; i < file_size; i++) {
                if(file_data[i] == '\n') {
                    uart_send('\r');
                }
                uart_send(file_data[i]);
                
            }
            uart_puts("\n");
            break;
        }
        cpio_address += file_size;
    }
}

void list_file() {
    volatile unsigned char *cpio_address = cpio_address_base;
    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = (char*)cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            break;
        }
        uart_puts(path_name);
        uart_puts("\n");

        cpio_address += name_size;
        // unsigned char *file_data = (unsigned char *)cpio_address;
        cpio_address += file_size;
    }
    char target[100];
    getFileData(target);
}

void load_user_program() {
    char target[100];
    uart_puts("Please enter file name: ");
    uart_read_line(target, 1);
    uart_send('\r');
    
    volatile unsigned char *prog_addr = (unsigned char *) 0x100000;
    unsigned long stack_top = (unsigned long) prog_addr + PROCESS_SIZE;
    volatile unsigned char *cpio_address = cpio_address_base;

    while(1) {
        int file_size = 0;
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = (char *)cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            uart_puts("No such file\n");
            break;
        }

        cpio_address += name_size;
        unsigned char *file_data = (unsigned char *)cpio_address;
        if(!strcmp(path_name, target)) {
            for(int i = 0; i < file_size; i++) {
                prog_addr[i] = file_data[i];
            }
            uart_puts("loading...\n");
            run_user_program((unsigned long)prog_addr, stack_top);
        }
        cpio_address += file_size;
    }
}

unsigned long argv_puts(char **argv, unsigned long stack_top) {
    int cnt1=0,cnt2=0;
	for(int i=0;;++i){
		cnt1++;//with null
		if(!argv[i])break;

		for(int j=0;;++j){
			cnt2++;//with null
			if(!argv[i][j])break;
		}
	}

	int sum=8+8+8*cnt1+cnt2;
	stack_top = (stack_top-sum);
	//alignment
	stack_top = stack_top-(stack_top&15);

	char* tmp=(char*)stack_top;
	*(unsigned long*)tmp=cnt1-1;
	tmp+=8;
	*(unsigned long*)tmp=(unsigned long)(tmp+8);
	tmp+=8;
	char* buffer=tmp+8*cnt1;
	for(int i=0;i<cnt1;++i){
		if(i+1==cnt1){
			*(unsigned long*)tmp=0;
		}else{
			*(unsigned long*)tmp=(unsigned long)buffer;
			tmp+=8;
			for(int j=0;;++j){
				*buffer=argv[i][j];
				buffer++;
				if(!argv[i][j])break;
			}
		}
	}
	return stack_top;
}

unsigned long load_user_program_withArgv(char *name, char **argv) {
    uart_puts("Please enter app load address (Hex): ");
    unsigned long *prog_addr = (unsigned long *)uart_getX(1);
    unsigned long stack_top = (unsigned long)prog_addr + PROCESS_SIZE;
    volatile unsigned char *cpio_address = cpio_address_base;
    unsigned long * file_data = NULL;
    int file_size = 0;

    while(1) {
        file_size = 0;        
        int name_size = 0;
        cpio_address += 54;
        file_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);
        cpio_address += 40;
        name_size = atoi(subStr((unsigned char *)cpio_address, 8), 16);

        name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
        file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
        cpio_address += 16;

        char *path_name = (char *)cpio_address;
        if(!strcmp(path_name, "TRAILER!!!")) {
            uart_puts("No such file\n");
            return 0;
        }

        cpio_address += name_size;
        file_data = (unsigned long *)cpio_address;
        if(!strcmp(path_name, name)) {
            uart_puts("find ");
            uart_puts(path_name);
            uart_puts("\n");
            break;
        }
        cpio_address += file_size;
    }

    for(int i = 0; i < file_size; i++) {
        prog_addr[i] = file_data[i];
    }
    uart_puts("loading user file with argv...\n");
    stack_top = argv_puts(argv, stack_top);
    run_user_program((unsigned long)prog_addr, stack_top);
    return 1;
}

void *get_cpio_base() {
    return (void *)cpio_address_base;
}

f_prop* get_file_property(void* addr) {
    int file_size = 0;
    int name_size = 0;
    int mode = 0;
    addr += 14;
    mode = atoi(subStr((unsigned char *)addr, 8), 16);
    unsigned long file_mode = (unsigned long)mode >> 12;
    addr += 40;
    file_size = atoi(subStr((unsigned char *)addr, 8), 16);
    addr += 40;
    name_size = atoi(subStr((unsigned char *)addr, 8), 16);

    name_size += (name_size+110) % 4 != 0 ? 4 - (name_size+110) % 4 : 0;
    file_size += file_size % 4 != 0 ? 4 - file_size % 4 : 0;
        
    addr += 16;

    char *path_name = (char*)addr;

    addr += name_size;
    unsigned char *file_data = (unsigned char *)addr;
    addr += file_size;
    
    f_prop *ret = kmalloc(sizeof(f_prop));
    ret->fname = path_name;
    ret->fdata = file_data;
    ret->nsize = name_size;
    ret->fsize = file_size;
    ret->fmode = file_mode;
    
    return ret;
}