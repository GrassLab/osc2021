#include "uart.h"
#include "utils.h"
#include "string.h"

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

unsigned long argvPut(char** argv,unsigned long ret){
	int cnt1=0,cnt2=0;
	for(int i=0;;++i){
		cnt1++;//with null
		if(argv[i] == NULL)break;

		for(int j=0;;++j){
			cnt2++;//with null
			if(argv[i][j] == NULL)break;
		}
	}

	int sum=8+8+8*cnt1+cnt2;
	ret=(ret-sum);
	//alignment
	ret=ret-(ret&15);

	char* tmp=(char*)ret;
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
	return ret;
}