#include "cpio.h"
#include "uart.h"
#include "utils.h"

#define CPIO_LOAD_ADDR 0x20000000
//#define CPIO_LOAD_ADDR 0x8000000

const unsigned int header_offset = 110;
struct cpio_data file_list[20];
unsigned int file_count = 0;

void read_cpio_archive() {
    unsigned int pc = 112;
		char *cpio = (char*)CPIO_LOAD_ADDR;
    int count;
    while(1) {
        pc += header_offset;
        count = 0;
        while (*(cpio+pc)) {
            file_list[file_count].file_name[count] = *(cpio+pc);
            count++;
            pc++;
        }
        file_list[file_count].file_name[count] = '\0';
        if (!strcmp(file_list[file_count].file_name, "TRAILER!!!")) {
            break;
        }
        pc++;
        while(*(cpio+pc) == '\0') {pc++;}
        count = 0;
        while (*(cpio+pc)) {
            file_list[file_count].file_content[count] = *(cpio+pc);
            count++;
            pc++;
        }
        file_list[file_count].file_content[count] = '\0';
        pc++;
        while(*(cpio+pc) == '\0') {pc++;}
        file_count++;
    }
}
