#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/stringUtils.h"
#include "../include/shell.h"
#include "../include/initrd.h"
#include "../include/interrupt.h"

/* cpio hpodc format */
typedef struct {
    char magic[6]; //070701
    char ino[8];  //inodes number from disk
    char mode[8]; //specifies both the regular permissions and file type
    char uid[8]; //userid
    char gid[8]; //groupid
    char nlink[8];  //number of links to this file
    char mtime[8];  //modification time
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8]; //always set to 0 by writers and ignored by reader
} __attribute__((packed)) cpio_t; // tell compiler don't do the alignment in order to save memory space
/*
    convert hexadecimal string into decimal
 */
unsigned long hexToDex(char *s){
    unsigned long r = 0;
    for(int i = 0 ;i < 8 ; ++i){
        if(s[i] >= '0' && s[i] <= '9'){
            r = r* 16 + s[i]-'0';
        }else if(s[i]>= 'a' && s[i]<='f'){
            r = r * 16 + s[i]-'a'+10;
        }else if(s[i]>= 'A' && s[i]<='F'){
            r = r*16 + s[i]-'A'+10;
        }
    }
    return r;
}
/*
 align to multiple of 4
 */
void Align_4(void* size){
    unsigned long* x =(unsigned long*) size;
    if((*x)&3){
        (*x) += 4-((*x)&3);
    }
}
/*
 print file content
 */

static void printFilecontent(cpio_t *addr){
    unsigned long psize = hexToDex(addr->namesize), dsize=hexToDex(addr->filesize);
    unsigned long HPP = sizeof(cpio_t) + psize;
    Align_4(&HPP);
    Align_4(&dsize);

    char *data = (char*)((char*)addr + HPP);
    uart_puts("\n");
    uart_puts("---------contents----------\n");
    for(int i = 0; i < dsize ; ++i){
        uart_send(data[i]);
    }
    uart_puts("---------------------------\n");


}
cpio_t* findFile(cpio_t* addr, char* str){
    while(1){
        unsigned long psize=hexToDex(addr->namesize),dsize=hexToDex(addr->filesize);
        unsigned long HPP = sizeof(cpio_t) + psize;
        Align_4(&HPP);
        Align_4(&dsize);

        char *data = (char*)((char*)addr + HPP);
        if(compString((char*)(addr+1), "TRAILER!!!") == 0) break;
        if(compString((char*)(addr+1), str) == 0) return addr;
        addr=(cpio_t*)(data+dsize);
    }

    return 0;
}


void getName(char* target){
    uart_puts("Please enter file name: ");
    read_input(target);
}

void loadprog(){
    char pathname[100];
  getName(pathname);
  char addr[200];
  uart_puts("Please enter address (Hex):");
  read_input(addr);
  unsigned long addr_hex = getHexFromString(addr); 
    cpio_t *file_addr = findFile((cpio_t*)0x8000000, pathname);
    if(!file_addr){
        uart_puts("file not found\n");
        return;
    }

    unsigned long psize=hexToDex(file_addr->namesize),dsize=hexToDex(file_addr->filesize);
    unsigned long HPP = sizeof(cpio_t) + psize;
    Align_4(&HPP);
    Align_4(&dsize);

    char *data = (char*)((char*)file_addr + HPP);
    unsigned char *target = (unsigned char*)addr_hex;
    while(dsize--){
        *target = *data;
        target++;
        data++;
    }

    asm volatile("mov x0, 0x340  \n");
    asm volatile("msr spsr_el1, x0   \n");
    asm volatile("msr elr_el1, %0    \n"::"r"(addr_hex));
    asm volatile("msr sp_el0, %0    \n"::"r"(addr_hex));
    core_timer_enable();
    asm volatile("eret    \n");
}

unsigned long argvPut(char** argv,unsigned long ret){
    //uart_printf("before argvput:%x\n",ret);
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
    uart_printf("after argvput:%x\n",ret);
	return ret;
}
void junk(){
    uart_printf("junk\n");
    char buf[8];
    read_input(buf);
    return ;
}

unsigned long loadprogWithArgv(char *path, unsigned long a_addr, char **argv){
    cpio_t *file_addr = findFile((cpio_t*)0x8000000, path);
    if(!file_addr){
        uart_puts("Prog not found\n");
        return 0;
    }

    unsigned long psize=hexToDex(file_addr->namesize),dsize=hexToDex(file_addr->filesize);
    unsigned long HPP = sizeof(cpio_t) + psize;
    Align_4(&HPP);
    //Align_4(&dsize);
// uart_printf("dsize:%d\n",dsize);
    unsigned char *data = (unsigned char*)((unsigned char*)file_addr + HPP);
    //*task_addr = a_addr;
    unsigned char* tar = (unsigned char*)a_addr;
    while(dsize--){
        *tar = *data;
        tar++;
        data++;
    }

    unsigned long sp_addr = argvPut(argv, a_addr);

    asm volatile("mov x0, 0x340   \n"::);
    asm volatile("msr spsr_el1, x0   \n"::);
    asm volatile("msr elr_el1, %0   \n"::"r"(a_addr));
    asm volatile("msr sp_el0, %0   \n"::"r"(sp_addr));
   core_timer_enable();
    asm volatile("mrs x3, sp_el0   \n"::);
    asm volatile("ldr x0, [x3, 0]   \n"::);
    asm volatile("ldr x1, [x3, 8]   \n"::);
    asm volatile("eret    \n"::);
}

/**
 * List the contents of an archive
 */
void cpio()
{
    uart_puts("Size\t\tFilename\n");
    cpio_t* addr = (cpio_t*)0x8000000;
    // iterate on archive's contents
    // if it's a cpio archive. Cpio also has a trailer entry
    while(compString((char*)(addr+1),"TRAILER!!!")!=0) {
        unsigned long psize=hexToDex(addr->namesize),dsize=hexToDex(addr->filesize);
        unsigned long HPP = sizeof(cpio_t) + psize;

        Align_4(&HPP);
        Align_4(&dsize);

        char *data = (char*)((char*)addr + HPP);
        // print out meta information
        uart_hex(dsize);      // file size in hex
        uart_puts("\t");
        uart_puts((char *)(addr+1));      // filename
        uart_puts("\n");
        // jump to the next file
        addr=(cpio_t*)(data+dsize);
    }

    addr = (cpio_t*)0x8000000;
    char target[100];
    getName(target);
    cpio_t* entry = findFile(addr,target);
    if(entry){
        printFilecontent(entry);
    }else{
        uart_puts("no such file\n");
    }
}


