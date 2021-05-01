#include "cpio.h"
#include "utils.h"

#define CPIO_LOAD_ADDR 0x20000000
//#define CPIO_LOAD_ADDR 0x8000000
const unsigned int header_size = 110;

cpio_t file_list[20];
unsigned int file_count;

void read_cpio_archive() {
    unsigned int pc = 112;
		char *cpio_addr = (char*)CPIO_LOAD_ADDR;
    file_count = 0;

    while(1) {
        int mode = char_2_int(*(cpio_addr + pc + 14));
        for (int i = 1; i < 8; i++) {
            mode *= 16;
            mode += char_2_int(*(cpio_addr + pc + 14 + i));
        }
        if (!mode)
            break;
        /* Check if file is executable */
        if (((mode & 448) >> 6) % 2) {
            file_list[file_count].executable = true;
        } else {
            file_list[file_count].executable = false;
        }

        unsigned int file_size = char_2_int(*(cpio_addr + pc + 54));
        unsigned int name_size = char_2_int(*(cpio_addr + pc + 94));
        int remainder;
        for (int i = 1; i < 8; i++) {
            file_size *= 16;
            file_size += char_2_int(*(cpio_addr + pc + 54 + i));
            name_size *= 16;
            name_size += char_2_int(*(cpio_addr + pc + 94 + i));
        }
        file_list[file_count].file_size = file_size;
        if (file_size) {
            if((remainder = file_size % 4))
                file_size += (4 - remainder);
        }
        if ((remainder = (name_size + header_size) % 4))
            name_size += (4 - remainder);

        pc += header_size;
        /* File name */
        file_list[file_count].file_name = (char*)(cpio_addr + pc);
        /* File content */
        file_list[file_count].file_content = (char*)(cpio_addr + pc + name_size);
        pc += (file_size + name_size);
        *(cpio_addr + pc) = '\0';
        file_count++;
    }
}
