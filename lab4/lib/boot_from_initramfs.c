void boot_from_initramfs(){
    //volatile unsigned char *kernel = (unsigned char *) 0x8000000;
    volatile unsigned char *kernel = (unsigned char *) 0x20000000;
    volatile unsigned char *prog = (unsigned char *) 0x60000;
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
            return 0;

        if(strcmp(kernel + 0x6E, ".")){
            for(int i=0; i<file_size; i++){
                prog[i] = *(kernel + name_size + i);
            }
            asm volatile("bl eret_initramfs");    
        }

        kernel += file_size + name_size ;
    }

}
