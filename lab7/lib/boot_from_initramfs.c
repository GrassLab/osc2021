#include "uart.h"
#include "utils.h"
#include "string.h"
#include "cpio.h"

void boot_from_initramfs(char* path,unsigned long a_addr,char** argv,unsigned long* task_a_addr,unsigned long* task_a_size){
    volatile unsigned char *kernel = (unsigned char *) 0x8000000; //qemu
    //volatile unsigned char *kernel = (unsigned char *) 0x20000000; //raspi3
    volatile unsigned char *prog = (unsigned char *) a_addr;
    volatile unsigned char *filename;
    int file_size;
    int name_size;
    int file_size_offset    = 6+8+8+8+8+8+8;
    int name_size_offset    = 6+8+8+8+8+8+8+8+8+8+8+8;

    while(1){ 
        file_size = 0;
        name_size = 0;
        
        for(int i=0;i<8;i++){
            if(kernel[file_size_offset + i] >= 'A' && kernel[file_size_offset + i] <= 'F')
                file_size = file_size * 0x10 + ((int)kernel[file_size_offset + i]) - 'A' + 0xA;            
            else
                file_size = file_size * 0x10 + ((int)kernel[file_size_offset + i]) - '0' ;

            if(kernel[name_size_offset + i] >= 'A' && kernel[name_size_offset + i] <= 'F')
                name_size = name_size * 0x10 + ((int)kernel[name_size_offset + i]) - 'A' + 0xA;
            else
                name_size = name_size * 0x10 + ((int)kernel[name_size_offset + i]) - '0' ;         
        }

        name_size += 0x6E;

        if((file_size % 4) != 0)
            file_size += (4 - (file_size % 4));

        if((name_size % 4) != 0)
            name_size += (4 - (name_size % 4));

        filename = (unsigned char *) kernel + 0x6E;

        if(!strcmp(kernel + 0x6E, "TRAILER!!!"))
            return;

        if(!strcmp(kernel + 0x6E, path)){
            *task_a_addr=a_addr;
	        *task_a_size=file_size/0x1000*0x1000+0x1000;// align
            for(int i=0; i<file_size; i++){
                prog[i] = *(kernel + name_size + i);
            }
	        uart_puts("loading...\n");
	        unsigned long sp_addr=argvPut(argv,a_addr + *task_a_size);            
            eret_initramfs(a_addr, sp_addr, a_addr + *task_a_size);   
        }

        kernel += file_size + name_size ;
    }

}
