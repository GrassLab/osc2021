/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "../include/uart.h"

// add memory compare, gcc has a built-in for that, clang needs implementation
//#ifdef __clang__
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

/**
 * Helper function to convert ASCII octal number into binary
 * s string
 * n number of digits
 */
int oct2bin(char *s, int n)
{
    int r=0;
    while(n-->0) {
        r<<=3;
        r+=*s++-'0';
    }
    return r;
}

unsigned long hexToDex(char *s){
    unsigned long r = 0;
    for(int i = 0 ;i < 8 ; ++i){
        if(s[i] >= '0' && s[i] <= '9'){
            r = r* 16 + s[i]-'0';
        }else{
            r = r * 16 + s[i]-'a'+'10';
        }
    }
    return r;
}

void printFilecontent(cpio_t *addr){
    unsigned long size = hexToDex(addr->filesize);
    unsigned long ns = hexToDex(addr->namesize);
    //int ns = hexToDex(addr -> namesize);
    char* path = (char*)(addr+1);
    char *data = (char *)(path+ns);
        for(int i = 0; i < size ; ++i){
            uart_send(data[i]);
        }


//	char* path=(char*)(addr+1);
	//char* data=path+ns;
	//if(memcmp(path,"TRAILER!!",9)==0)return ;

//	uart_printf("Path: %s\n",path);
	//uart_puts("---Data---\n");
	//for(int i=0;i<size;++i){
	//	if(data[i])uart_send(data[i]);
	//}
	//uart_puts("----------\n");

}
cpio_t* findFile(cpio_t* buf, char* str){
    while(1){
    unsigned long size = hexToDex(buf->filesize);
    unsigned long ns = hexToDex(buf->namesize);
        char* path=(char*)(buf+1);
        char* data=path+ns;
        if(compString(path,"TRAILER!!!")==0)break;
        if(compString(path,str)==0)return buf;
        buf=(cpio_t*)(data+size);
    }

return 0;
}
/**
 * List the contents of an archive
 */
void initrd_list(char *buf)
{
    char *types[]={"regular", "link  ", "symlnk", "chrdev", "blkdev", "dircty", "fifo  ", "???   "};

    uart_puts("Type     Offset   Size     Access rights\tFilename\n");
        cpio_t* addr = (cpio_t*)0x8000000;
        cpio_t* cool = findFile(addr,"a.txt");
    // iterate on archive's contents
    // if it's a cpio archive. Cpio also has a trailer entry
    while(!memcmp(buf,"070701",6) && memcmp(buf+sizeof(cpio_t),"TRAILER!!",9)) {
        cpio_t *header = (cpio_t*)buf;
        int ns=oct2bin(header->namesize,8);
        int fs=oct2bin(header->filesize,8);
        // print out meta information
        uart_hex(oct2bin(header->mode,8));  // mode (access rights + type)
        uart_send(' ');
        uart_hex((unsigned int)((unsigned long)buf)+sizeof(cpio_t)+ns);
        uart_send(' ');
        uart_hex(fs);                       // file size in hex
        uart_send(' ');
        uart_hex(oct2bin(header->uid,8));   // user id in hex
        uart_send('.');
        uart_hex(oct2bin(header->gid,8));   // group id in hex
        uart_send('\t');
        uart_puts(buf+sizeof(cpio_t));      // filename
        uart_puts("\n");
        // jump to the next file
        buf+=(sizeof(cpio_t)+ns+fs);
        printFilecontent(cool);
    }
}


