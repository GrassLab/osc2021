#include "cpio.h"
#include "mystring.h"
#include "uart.h"


unsigned long parse_hex_str(char* s, unsigned int max_len) {
    unsigned long r=0;
    for(int i=0;i<max_len;i++) {
        r *= 16;
        if(s[i] >= '0' && s[i] <= '9') {
            r += s[i] - '0';
        }
        else if(s[i] >= 'a' && s[i] <= 'f') {
            r += s[i] - 'a' + 10;
        }
        else if(s[i] >= 'A' && s[i] <= 'F') {
            r += s[i] - 'A' + 10;
        }
        else {
            return r;
        }
    }
    return r;
}

unsigned long align_up(unsigned long n, unsigned long align) {
    return (n+align-1) & (~(align-1));
}

int parse_cpio_header(struct cpio_newc_header *header, char **filename,
        unsigned long *filesize, void **data, struct cpio_newc_header **next) {
    *filesize = parse_hex_str(header->c_filesize, sizeof(header->c_filesize));

    *filename = ((char*)header) + sizeof(struct cpio_newc_header);

    if(strncmp(*filename, CPIO_FOOTER_MAGIC, sizeof(CPIO_FOOTER_MAGIC)) == 0) {
        return 1;
    }

    unsigned long filename_length = parse_hex_str(header->c_namesize,
            sizeof(header->c_namesize));
    
    *data = (void*)align_up(((unsigned long)header) + sizeof(struct cpio_newc_header) +
            filename_length, 4);

    *next = (struct cpio_newc_header*)align_up(
                ((unsigned long)*data) + *filesize, 4);

    return 0;
}

void print_cpio() {
    void *cpio_base = (void *)0x8000000;
    char* filename;
    void* data;
    unsigned long filesize;
    struct cpio_newc_header *header, *next;

    char data_str[1000];

    int error = 0;

    header = cpio_base;
    while(1) {
        error = parse_cpio_header(header, &filename, &filesize, &data, &next);

        // error or trailer!!!
        if(error) break;

        // print data
        memcpy(data_str, data, filesize);
        data_str[filesize] = '\0';
        uart_putstr(filename);
        uart_putstr(": ");
        uart_putstr(data_str);
        uart_putstr("\n");

        header = next;
    }
}
