#ifndef CPIO_H
#define CPIO_H

void *get_cpio_file(const char *name);
void reserve_cpio();
unsigned long get_file_size(const void *cpio_file);
void *get_file_data(const void *cpio_file);

#endif