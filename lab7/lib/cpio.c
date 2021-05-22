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

#define CPIO_BASE ((cpio_header*)0x8000000) //qemu
//#define CPIO_BASE ((cpio_header*)0x20000000) //raspi3

unsigned long strToU(char* str){
	unsigned long ret=0;
	for(int i=0;i<8;++i){
		if(str[i]>='0'&&str[i]<='9'){
			ret=ret*16+str[i]-'0';
		}else if(str[i]>='a'&&str[i]<='f'){
			ret=ret*16+str[i]-'a'+10;
		}else if(str[i]>='A'&&str[i]<='F'){
			ret=ret*16+str[i]-'A'+10;
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

cpio_header* findEntry(cpio_header* addr,char* target){
	while(1){
		unsigned long psize=strToU(addr->c_namesize),dsize=strToU(addr->c_filesize);
		if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
		if(dsize&3)dsize+=4-(dsize&3);

		char* path=(char*)(addr+1);
		char* data=path+psize;
		if(strcmp(path,"TRAILER!!!")==0)break;
		if(strcmp(path,target)==0)return addr;
		addr=(cpio_header*)(data+dsize);
	}
	return 0;
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

void getName(char* target){
	uart_puts("Please enter file name: ");
	int cnt=0;
	while(1){
		target[cnt++]=uart_getc();
		uart_send(target[cnt-1]);
		if(target[cnt-1]=='\n')break;
	}
	target[--cnt]=0;
}

void dumpArchive(){
	//cpio_header* addr=(cpio_header*)getBase();
	cpio_header* addr=CPIO_BASE;
	char target[100];getName(target);
	cpio_header* ret=findEntry(addr,target);
	if(ret)dumpEntry(ret);
}

void* fbaseGet(){
	return (void*)CPIO_BASE;
}

char* fdataGet(void* _addr,unsigned long* size){
	cpio_header* addr=(cpio_header*)_addr;
	unsigned long psize=strToU(addr->c_namesize),dsize=strToU(addr->c_filesize);
	*size=dsize;
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	if(dsize&3)dsize+=4-(dsize&3);

	char* path=(char*)(addr+1);
	char* data=path+psize;

	return data;
}

int fmodeGet(void* _addr){
	cpio_header* addr=(cpio_header*)_addr;
	unsigned long tmp=strToU(addr->c_mode)>>12;
	if(tmp==4){
		return 1;//dir
	}else if(tmp==8){
		return 2;//file
	}
	return -1;
}

char* fnameGet(void* _addr,unsigned long* size){
	cpio_header* addr=(cpio_header*)_addr;
	unsigned long psize=strToU(addr->c_namesize),dsize=strToU(addr->c_filesize);
	*size=psize;
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	if(dsize&3)dsize+=4-(dsize&3);

	char* path=(char*)(addr+1);
	//char* data=path+psize;

	return path;
}

void* nextfGet(void* _addr){
	cpio_header* addr=(cpio_header*)_addr;
	unsigned long psize=strToU(addr->c_namesize),dsize=strToU(addr->c_filesize);
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	if(dsize&3)dsize+=4-(dsize&3);

	char* path=(char*)(addr+1);
	char* data=path+psize;

	if(strcmp(path,"TRAILER!!!")==0)return 0;
	addr=(cpio_header*)(data+dsize);
	return addr;
}

void fDump(){
	void* addr=CPIO_BASE;
	while(addr){
		unsigned long tmp;
		char* str=fnameGet(addr,&tmp);
		uart_printf("%d, %s\n",tmp,str);
		addr=nextfGet(addr);
	}
}

void loadApp(char* path,unsigned long a_addr,unsigned long a_size){
	cpio_header* ret=findEntry(CPIO_BASE,path);
	if(!ret){
		uart_puts("App not found!\n");
		return;
	}
	unsigned long psize=strToU(ret->c_namesize);
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	unsigned char* data=(unsigned char*)(ret+1)+psize;

	unsigned char* target=(unsigned char*)a_addr;
	while(a_size--){
		*target=*data;
		target++;
		data++;
	}

	uart_puts("loading...\n");

	//change exception level
	//asm volatile("mov x0, 0x3c0			\n");//disable interrupt
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(a_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(a_addr));
	//asm volatile("mov x0, #(3 << 20)	\n");
	//asm volatile("msr cpacr_el0, x0		\n");

	asm volatile("eret					\n");
}

unsigned long argvPut(char** argv,unsigned long ret){
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

void loadApp_with_argv(char* path,unsigned long a_addr,char** argv,unsigned long* task_a_addr,unsigned long* task_a_size){
	cpio_header* ret=findEntry(CPIO_BASE,path);
	if(!ret){
		uart_puts("App not found!\n");
		return;
	}
	unsigned long a_size=strToU(ret->c_filesize);
	unsigned long psize=strToU(ret->c_namesize);
	if((sizeof(cpio_header)+psize)&3)psize+=4-((sizeof(cpio_header)+psize)&3);
	unsigned char* data=(unsigned char*)(ret+1)+psize;

	*task_a_addr=a_addr;
	*task_a_size=a_size;

	unsigned char* target=(unsigned char*)a_addr;
	while(a_size--){
		*target=*data;
		target++;
		data++;
	}

	uart_puts("loading...\n");

	unsigned long sp_addr=argvPut(argv,a_addr);
	/*
	uart_printf("%d\n",*(unsigned long*)sp_addr);
	char** tmp=*(char***)(sp_addr+8);
	uart_printf("%x %x\n",tmp,tmp[0]);
	for(int i=0;;++i){
		if(!tmp[i])break;
		uart_printf("%d %s\n",i,tmp[i]);
	}
	while(1){}
	*/

	//change exception level
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(a_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(sp_addr));

	asm volatile("mrs x3, sp_el0		\n"::);
	asm volatile("ldr x0, [x3, 0]		\n"::);
	asm volatile("ldr x1, [x3, 8]		\n"::);

	asm volatile("eret					\n");
}