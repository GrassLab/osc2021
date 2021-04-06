#ifndef CPIO_H
#define CPIO_H

#define CPIO_FOOTER_MAGIC "TRAILER!!!"

struct cpio_newc_header {
    char    c_magic[6];
    char    c_ino[8];
    char    c_mode[8];
    char    c_uid[8];
    char    c_gid[8];
    char    c_nlink[8];
    char    c_mtime[8];
    char    c_filesize[8];
    char    c_devmajor[8];
    char    c_devminor[8];
    char    c_rdevmajor[8];
    char    c_rdevminor[8];
    char    c_namesize[8];
    char    c_check[8];
};

unsigned long parse_hex_str(char* s, unsigned int max_len);
unsigned long align_up(unsigned long n, unsigned long align);
int parse_cpio_header(struct cpio_newc_header *header, char **filename,
        unsigned long* filesize, void **data, struct cpio_newc_header **next);

void print_cpio();

#endif
