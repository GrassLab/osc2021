#ifndef _CPIO_H_
#define  _CPIO_H_

struct cpio_data {
    char file_name[20];
    char file_content[100];
};

extern struct cpio_data file_list[20];
extern unsigned int file_count;

void read_cpio_archive();

#endif
