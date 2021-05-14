#include "sd.h"
#include "allocator.h"
#include "uart.h"
#include "error.h"

/*
Note: https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system

Reserved sectors	32
FAT Region			table_num=disk_size/block_size/(512/4)=(2^20/2^9/128)*1=16
Data Region
*/

int getPartition(){
	unsigned char* buf=(unsigned char*)falloc(512);
	readblock(0,buf);

	if(buf[510]!=0x55||buf[511]!=0xaa)ERROR("invalid MBR signature!");
	if(buf[446]!=0x80)ERROR("invalid partition!");
	unsigned char* partition_entry=buf+446;
	unsigned int start=*(unsigned int*)(partition_entry+8);
	unsigned int size=*(unsigned int*)(partition_entry+12);
	//uart_printf("%d %d\n",start,size);

	ffree((unsigned long)buf);
	return start;
}

void fat_Setup(){
	int partition=getPartition();
}