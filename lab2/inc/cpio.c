#include "cpio.h"
#include "uart.h"

typedef struct{//cpio_newc_header
	char c_magic[6];
	char c_ino[8];
	char c_mode[8];
	char c_uid[8];
	char c_gid[8];
	char c_nlink[8];
	char c_mtime[8];
	char c_filesize[8];
	char c_devmajor[8];
	char c_devminor[8];
	char c_rdevmajor[8];
	char c_rdevminor[8];
	char c_namesize[8];
	char c_check[8];
}cpio_header;

unsigned long strToU(char* str){
	unsigned long ret=0;
	for(int i=0;i<8;++i){
		if(str[i]>='0'&&str[i]<='9'){
			ret=ret*16+str[i]-'0';
		}else{
			ret=ret*16+str[i]-'a'+10;
		}
	}
	return ret;
}

cpio_header* dumpEntry(cpio_header* addr){
	unsigned long psize=strToU(addr->c_namesize),dsize=strToU(addr->c_filesize);
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	if(dsize&3)dsize+=4-(dsize&3);

	char* path=(char*)(addr+1);
	char* data=path+psize;
	if(strcmp(path,"TRAILER!!!")==0)return 0;

	uart_printf("Path: %s\n",path);
	uart_printf("---Data---\n");
	for(int i=0;i<dsize;++i){
		if(data[i])uart_send(data[i]);
	}
	uart_printf("----------\n");

	char* ret=data+dsize;
	return (cpio_header*)ret;
}

cpio_header* getBase(){
	unsigned long addr=0;
	char c;
	uart_puts("Please enter archive load address (Hex): ");//qemu: 0x8000000
	do{
		c=uart_getc();
		if(c>='0'&&c<='9'){
			addr=addr*16+c-'0';
		}else if(c>='a'&&c<='f'){
			addr=addr*16+c-'a'+10;
		}else if(c>='A'&&c<='F'){
			addr=addr*16+c-'A'+10;
		}
	}while(c!='\n');
	uart_printf("0x%x\n",addr);
	return (cpio_header*)addr;
}

void dumpArchive(){
	//cpio_header* addr=(cpio_header*)getBase();
	cpio_header* addr=(cpio_header*)0x8000000;
	while(addr){
		uart_send('\n');
		addr=dumpEntry(addr);
	}
}