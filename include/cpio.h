#ifndef _CPIO_H_
#define  _CPIO_H_

#include "type.h"

typedef enum {
    DIR,
    FILE
} file_type_t;

typedef struct {
    char         *file_name;
    char         *file_content;
    uint32_t     file_size;
    bool_t       executable;
    file_type_t   file_type;
} cpio_t;

extern cpio_t file_list[20];
extern uint32_t file_count;

void read_cpio_archive();
int search_file(const char *);

#endif
