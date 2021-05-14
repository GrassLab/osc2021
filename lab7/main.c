#include "inc/uart.h"
#include "inc/reboot.h"
#include "inc/mailbox.h"
#include "inc/cpio.h"
#include "inc/allocator.h"
#include "inc/thread.h"
#include "inc/tmpfs.h"
#include "inc/sd.h"
#include "inc/fat.h"

#define min(a,b) ((a)<(b)?(a):(b))

void moveImg(){//should use register
	asm volatile("cbz x14, endmove		\n"::);
	asm volatile("ldrb w0, [x13], #1	\n"::);
	asm volatile("strb w0, [x12], #1	\n"::);
	asm volatile("sub x14, x14, #1		\n"::);
	asm volatile("br x10				\n"::);
	asm volatile("endmove:				\n"::);
	asm volatile("br x11				\n"::);
}

void loadImg(){
	extern unsigned long* __prog_start,__prog_end;
	unsigned long c_addr,c_size;
	c_addr=(unsigned long)&__prog_start;
	c_size=(unsigned long)(&__prog_end)-(unsigned long)(&__prog_start);//include stack

	unsigned long k_addr=0,k_size=0;
	char c;
	uart_puts("Please enter kernel load address (Hex): ");
	do{
		c=uart_getc();
		if(c>='0'&&c<='9'){
			k_addr=k_addr*16+c-'0';
		}else if(c>='a'&&c<='f'){
			k_addr=k_addr*16+c-'a'+10;
		}else if(c>='A'&&c<='F'){
			k_addr=k_addr*16+c-'A'+10;
		}
	}while(c!='\n');
	uart_printf("0x%x\n",k_addr);

	uart_puts("Please enter kernel size (Dec): ");
	do{
		c=uart_getc();
		if(c>='0'&&c<='9'){
			k_size=k_size*10+c-'0';
		}
	}while(c!='\n');
	uart_printf("%d\n",k_size);

	uart_puts("Please send kernel image now...\n");
	if(c_addr>k_addr+k_size||c_addr+c_size<k_addr){//no overlap
		unsigned char* target=(unsigned char*)k_addr;
		while(k_size--){
			*target=uart_getb();
			target++;
			uart_send('.');
		}

		uart_puts("loading...\n");
		asm volatile("br %0\n"::"r"(k_addr));
	}else{//overlap
		unsigned long tmp_addr=min(c_addr,k_addr)-(c_size+k_size);
		tmp_addr&=(~0x3);//alignment
		unsigned char* target=(unsigned char*)tmp_addr;
		for(int i=0;i<c_size;++i){//cur prog
			*target=((unsigned char*)c_addr)[i];
			target++;
		}
		for(int i=0;i<k_size;++i){//new prog
			*target=uart_getb();
			target++;
			uart_send('.');
		}

		uart_puts("moving...\n");
		unsigned long delta=c_addr-tmp_addr;
		asm volatile("mov x10, %0	\n"::"r"((unsigned long)moveImg-delta));//copy-loop address
		asm volatile("mov x11, %0	\n"::"r"(k_addr));//load address
		asm volatile("mov x12, %0	\n"::"r"(k_addr));//copy dst
		asm volatile("mov x13, %0	\n"::"r"(tmp_addr+c_size));//copy src
		asm volatile("mov x14, %0	\n"::"r"(k_size));//copy size
		asm volatile("br x10		\n"::);
	}
}

void shell(){
	uart_puts("Welcome!\n");
	char buffer[1000];
	int cnt;
	while(1){
		uart_puts("$ ");

		cnt=0;
		do{
			buffer[cnt++]=uart_getc();
			uart_send(buffer[cnt-1]);
		}while(buffer[cnt-1]!='\n');
		buffer[--cnt]=0;

		if(strcmp(buffer,"help")==0){
			uart_puts("commands:\n");
			uart_puts("          help\n");
			uart_puts("          hello\n");
			uart_puts("          reboot\n");
			uart_puts("          loadimg\n");//current program will no longer be used(allow overlap)
			uart_puts("          archive\n");
			uart_puts("          falloc\n");
			uart_puts("          ffree\n");
			uart_puts("          dalloc\n");
			uart_puts("          dfree\n");
			uart_puts("          loadapp\n");//current program will handle exceptions(not allow overlap)
			uart_puts("          lab5-1\n");
			uart_puts("          lab5-2\n");
			uart_puts("          fdump\n");
		}else if(strcmp(buffer,"hello")==0){
			uart_puts("Hello World!\n");
		}else if(strcmp(buffer,"reboot")==0){
			uart_puts("rebooting...\n");
			reboot();
		}else if(strcmp(buffer,"loadimg")==0){
			loadImg();
		}else if(strcmp(buffer,"archive")==0){
			dumpArchive();
		}else if(strcmp(buffer,"falloc")==0){
			uart_printf("Size(Hex): ");
			unsigned long ret=uart_getX(1);
			falloc(ret);
		}else if(strcmp(buffer,"ffree")==0){
			uart_printf("Address(Hex): ");
			unsigned long ret=uart_getX(1);
			ffree(ret);
		}else if(strcmp(buffer,"dalloc")==0){
			uart_printf("Size(Hex): ");
			unsigned long ret=uart_getX(1);
			dalloc(ret);
		}else if(strcmp(buffer,"dfree")==0){
			uart_printf("Address(Hex): ");
			unsigned long ret=uart_getX(1);
			dfree(ret);
		}else if(strcmp(buffer,"loadapp")==0){
			char path[100];
			unsigned long a_addr,a_size;
			uart_puts("APP path: ");
			uart_gets(path,100,1);
			uart_puts("Please enter app load address (Hex): ");
			a_addr=uart_getX(1);
			uart_puts("Please enter app size (Dec): ");
			a_size=uart_getU(1);
			loadApp(path,a_addr,a_size);
		}else if(strcmp(buffer,"lab5-1")==0){
			threadTest1();
		}else if(strcmp(buffer,"lab5-2")==0){
			threadTest2();
		}else if(strcmp(buffer,"fdump")==0){
			fDump();
		}else if(strcmp(buffer,"lab6-1")==0){
			char buf[100];
			//RW test
			file* f=vfs_open("dir/dirdir/f3",0);
			for(int i=0;i<10;++i)vfs_write(f,"12345",5);
			vfs_close(f);
			f=vfs_open("dir/dirdir/f3",0);
			int n=vfs_read(f,buf,100);
			buf[n]=0;
			uart_printf("%d: %s\n",n,buf);
			vfs_close(f);

			//create test
			file* a=vfs_open("dir/hello",O_CREAT);
			file* b=vfs_open("dir/world",O_CREAT);
			vfs_write(a,"Hello ",6);
			vfs_write(b,"World!",6);
			vfs_close(a);
			vfs_close(b);
			b=vfs_open("dir/hello",0);
			a=vfs_open("dir/world",0);
			int sz;
			sz=vfs_read(b,buf,100);
			sz+=vfs_read(a,buf+sz,100);
			buf[sz]=0;
			uart_printf("%s\n",buf);//should be Hello World!
			vfs_close(a);
			vfs_close(b);

			//ls test
			f=vfs_open("dir",0);
			while(1){
				n=vfs_read(f,buf,100);
				if(n==0)break;
				buf[n]=0;
				uart_printf("...%s\n",buf);
			}
			vfs_close(f);
		}else if(strcmp(buffer,"lab6-2")==0){
			threadTest();
		}else{
			uart_puts("Error: No such command \"");
			uart_puts(buffer);
			uart_puts("\".\n");
		}
	}
}

void printHWInfo(){
	unsigned int v[2];

	if(getBoardRevision(v)){
		uart_printf("board revision: 0x%x\n",v[0]);
	}else{
		uart_printf("Error: getBoardRevision() fail.\n");
	}

	if(getVCMEM(v)){
		uart_printf("VC memory base address: 0x%x\n",v[0]);
		uart_printf("VC memory size: %d\n",v[1]);
	}else{
		uart_printf("Error: getVCMEM() fail.\n");
	}
}

void main(){
	uart_init();
	printHWInfo();
	allocator_init();
	sd_init();

	//TODO: fat
	fat_Setup();
	//

	vfs_init(tmpfs_Setup);

	shell();
}