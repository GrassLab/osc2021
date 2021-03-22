#ifndef CPIO_H
#define CPIO_H

int hex2int(char *hex);
int round2four(int origin, int option);
void read(char **address, char *target, int count);

struct cpio_newc_header
{
    char c_magic[6];
    char c_filesize[8];
    char c_namesize[8];
};

void cpio_parse_header(char **address, char *file_name, char *file_content);
void cpio_ls();
void cpio_find_file(char file_name_to_find[]);

#endif