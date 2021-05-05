#ifndef CPIO_H
#define CPIO_H

#include "types.h"

struct cpio_newc_header {
    char	   c_magic[6];
    char	   c_ino[8];
    char	   c_mode[8];
    char	   c_uid[8];
    char	   c_gid[8];
    char	   c_nlink[8];
    char	   c_mtime[8];
    char	   c_filesize[8];
    char	   c_devmajor[8];
    char	   c_devminor[8];
    char	   c_rdevmajor[8];
    char	   c_rdevminor[8];
    char	   c_namesize[8];
    char	   c_check[8];
};

class CPIO {
public:
    CPIO(cpio_newc_header* header);
    uint64_t filesize;
    uint64_t namesize;
    char* filename;
    char* filecontent;
    cpio_newc_header* header;
    cpio_newc_header* next();
private:
    int Hex2int(uint8_t hex);
};

#endif
