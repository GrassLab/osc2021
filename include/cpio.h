#ifndef _CPIO_H_
#define  _CPIO_H_

typedef struct {
    char file_name[20];
    char file_content[100];
} cpio_t;

extern cpio_t file_list[20];
extern unsigned int file_count;

void read_cpio_archive();

#endif
