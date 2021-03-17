#include "../include/gpio.h"
#include "../include/uart.h"
#include "../include/stringUtils.h"
// add memory compare, gcc has a built-in for that, clang needs implementation
//#ifdef __clang__
//
//
int memcmp(void *s1, void *s2, int n)
{
    unsigned char *a=s1,*b=s2;
    while(n-->0){ if(*a!=*b) { return *a-*b; } a++; b++; }
    return 0;
}
//#else
//#define memcmp __builtin_memcmp
//#endif

/* cpio hpodc format */
typedef struct {
    char magic[6];
    char ino[8];
    char mode[8];
    char uid[8];
    char gid[8];
    char nlink[8];
    char mtime[8];
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8];
} __attribute__((packed)) cpio_t;

unsigned long hexToDex(char *s){
    unsigned long r = 0;
    for(int i = 0 ;i < 8 ; ++i){
        if(s[i] >= '0' && s[i] <= '9'){
            r = r* 16 + s[i]-'0';
        }else{
            r = r * 16 + s[i]-'a'+10;
        }
    }
    return r;
}

void printFilecontent(cpio_t *addr){
    unsigned long psize=hexToDex(addr->namesize),dsize=hexToDex(addr->filesize);
    if((sizeof(cpio_t)+psize)&3)psize+=4-((sizeof(cpio_t)+psize)&3);
    if(dsize&3)dsize+=4-(dsize&3);

    char* path = (char*)(addr+1);
    char *data = (char *)(path+psize);
    uart_puts("\n");
    uart_puts("---------contents----------\n");
    for(int i = 0; i < dsize ; ++i){
        uart_send(data[i]);
    }
    while(*AUX_MU_LSR&0x01){
        char c = (char)(*AUX_MU_IO);
    }
    while(*AUX_MU_LSR&0x01){
		char tmp=(char)(*AUX_MU_IO);
	}
    uart_puts("---------------------------\n");


}
cpio_t* findFile(cpio_t* buf, char* str){
    while(1){
        //For Aligning
        unsigned long psize=hexToDex(buf->namesize),dsize=hexToDex(buf->filesize);
        if((sizeof(cpio_t)+psize)&3)psize+=4-((sizeof(cpio_t)+psize)&3);
        if(dsize&3)dsize+=4-(dsize&3);

        char* path=(char*)(buf+1);
        char* data=path+psize;
        if(compString(path,"TRAILER!!!")==0)break;
        if(compString(path,str)==0)return buf;
        buf=(cpio_t*)(data+dsize);
    }

    return 0;
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
/**
 * List the contents of an archive
 */
void cpio()
{
    uart_puts("Size\t\tFilename\n");
    cpio_t* addr = (cpio_t*)0x2000000;
    // iterate on archive's contents
    // if it's a cpio archive. Cpio also has a trailer entry
    while(compString((char*)(addr+1),"TRAILER!!!")!=0) {
        unsigned long psize=hexToDex(addr->namesize),dsize=hexToDex(addr->filesize);
        if((sizeof(cpio_t)+psize)&3)psize+=4-((sizeof(cpio_t)+psize)&3);
        if(dsize&3)dsize+=4-(dsize&3);

        char* path=(char*)(addr+1);
        char* data=path+psize;
        // print out meta information
        uart_hex(dsize);                       // file size in hex
        uart_puts("\t");
        uart_puts(path);      // filename
        uart_puts("\n");
        // jump to the next file
        addr=(cpio_t*)(data+dsize);
    }

    addr = (cpio_t*)0x2000000;
    char target[100];
    getName(target); 
    cpio_t* entry = findFile(addr,target);
    if(entry){
        printFilecontent(entry);
    }else{
        uart_puts("no such file\n"); 
    }
}


