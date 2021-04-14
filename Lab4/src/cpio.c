#include "cpio.h"
#include "uart.h"
#include "my_string.h"
#include "stddef.h"
#include "stdint.h"
unsigned int c2i(char c){
	if (c >= '0' && c <= '9') return      c - '0';
 	if (c >= 'A' && c <= 'F') return 10 + c - 'A';
  	if (c >= 'a' && c <= 'f') return 10 + c - 'a';
  	return -1;
}

unsigned h2i(char *buf, int len){
	unsigned int num = 0;
	for(int i = 0; i < len; i++){
		num <<= 4;
		num += (c2i(buf[i]));
	}
	return num;
}
size_t align_up(size_t size, int alignment) {
 	return (size + alignment - 1) & -alignment;
}
char ls(void *cpiobuf){
	uintptr_t ptr = (uintptr_t)cpiobuf;
	const struct cpio_newc_header *header;
	uart_printf("list all \n");
	while(1){
		header = (struct cpio_newc_header *)ptr;
		if (strncmp(header->c_magic, "070701", 6)) {
      			return;
    		}

	    	unsigned namesize = h2i(header->c_namesize, 8);
	    	unsigned filesize = h2i(header->c_filesize, 8);
	    	if (!strcmp(header->content_buf, "TRAILER!!!")) {
	      		break;
	    	}

	    	ptr += sizeof(struct cpio_newc_header);
	    	uart_printf("%s\n", (char *)ptr);
	    	ptr = align_up(ptr + namesize, 4);
	    	ptr = align_up(ptr + filesize, 4);
	}
}

char *cpio_content(const char *content, void *cpiobuf, size){
	uintptr_t ptr = (uintptr_t)cpiobuf;
	const struct cpio_newc_header *header;
	while(1){
		header = (struct cpio_newc_header *)ptr;
		if (strncmp(header->c_magic, "070701", 6)) {
      		return NULL;
    	}
    	if (!strcmp(header->content_buf, "TRAILER!!!")) {
      		return NULL;
    	}

    	unsigned namesize = h2i(header->c_namesize, 8);
    	unsigned filesize = h2i(header->c_filesize, 8);
    	unsigned mod = h2i(header->c_mode, 8);

    	if (!strcmp(header->content_buf, content)) {
      		if (S_ISDIR(mod))return "directory";

      		if (filesize) {
        		ptr += sizeof(struct cpio_newc_header);
        		ptr = align_up(ptr + namesize, 4);
        		return (char *)ptr;
      		}
     	 return "";

    	}

    	else {
      		ptr += sizeof(struct cpio_newc_header);
      		ptr = align_up(ptr + namesize, 4);
      		ptr = align_up(ptr + filesize, 4);
    	}
	}

}


void cpio_load_program(char *target, uint64_t target_addr, void *cpiobuf){
	uintptr_t ptr = (uintptr_t)cpiobuf;
	const struct cpio_newc_header *header;

	while(1){
		header = (struct cpio_newc_header *ptr)ptr;
		if (strncmp(header->c_magic, "070701", 6)) {
      		return ;
    	}
    	if (!strcmp(header->content_buf, "TRAILER!!!")) {
      		return ;
    	}

    	unsigned namesize = h2i(header->c_namesize, 8);
    	unsigned filesize = h2i(header->c_filesize, 8);
    	if (!strcmp(header->content_buf, content)) {
      		if (S_ISDIR(mod))return "directory";

      		if (filesize) {
      			char *content = (char *)ptr;
        		for(unsigned long long i = 0; i < filesize; i++){
        			target[i] = content[i];
        		}
        		return;
      		}
     	 return ;

    	}
    	else {
      		ptr += sizeof(struct cpio_newc_header);
      		ptr = align_up(ptr + namesize, 4);
      		ptr = align_up(ptr + filesize, 4);
    	}
	}

}