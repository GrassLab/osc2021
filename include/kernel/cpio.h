#ifndef _CPIO_H
#define _CPIO_H

// qemu use 0x8000000 as default
#ifdef EMU
#define RAMFS_ADDR (0x8000000)
#else
#define RAMFS_ADDR (0x100000)
#endif

typedef struct cpio_newc_header CPIO_NEWC_HEADER;
typedef enum CPIO_ATTR CPIO_ATTR;

struct cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
} __attribute__((packed));

enum CPIO_ATTR {
    C_MAGIC,
    C_INO,
    C_MODE,
    C_UID,
    C_GID,
    C_NLINK,
    C_MTIME,
    C_FILESIZE,
    C_DEVMAJOR,
    C_DEVMINOR,
    C_RDEVMAJOR,
    C_RDEVMINOR,
    C_NAMESIZE,
    C_CHECK
};


CPIO_NEWC_HEADER * cpio_find_addr(CPIO_NEWC_HEADER *pCurrentFile, char *targetName);
int cpio_attr_value(CPIO_NEWC_HEADER *pRoot, CPIO_ATTR attr);
void cpio_read(char *path);
void cpio_exec(char *path);
void * cpio_content_addr(CPIO_NEWC_HEADER *targetAddr);

#endif