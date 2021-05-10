#ifndef _CPIO_H_
#define  _CPIO_H_

#include "type.h"

typedef struct {
    char *file_name;
    char *file_content;
    unsigned int file_size;
    bool_t executable;
} cpio_t;

extern cpio_t file_list[20];
extern unsigned int file_count;

void read_cpio_archive();
int search_file(const char *);

#endif
