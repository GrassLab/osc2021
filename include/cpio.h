#ifndef CPIO_H
#define CPIO_H

void *get_cpio_file(char *name);
void reserve_cpio();
unsigned long get_file_size(void *cpio_file);
void *get_file_data(void *cpio_file);

#endif