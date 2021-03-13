#ifndef CPIO_H
#define CPIO_H

int hex2int(char *hex);
int round2four(int origin, int option);

struct cpio_newc_header
{
    char c_magic[6];
    char c_filesize[8];
    char c_namesize[8];
};

void parse_cpio();
void read(char **address, char *target, int count);

#endif