#ifndef _CPIO_H_
#define _CPIO_H_
#include "uart.h"
#include "utils.h"
#include "timer.h"

#define CPIO_ADDR ((char*)0x8000000)    //QEMU(0x8000000)
#define NEW_ADDR ((char*)0x70000)    //QEMU(0x8000000)
#define NEW_SP ((char*)0x100000) 

//#define CPIO_ADDR ((char*)0x30000000)


/* Magic identifiers for the "cpio" file format. */
#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"
#define CPIO_ALIGNMENT 4


#define CPIO_SIZE 110

struct cpio_newc_header {
    char c_magic[6];      /* Magic header '070701'. */
    char c_ino[8];        /* "i-node" number. */
    char c_mode[8];       /* Permisions. */
    char c_uid[8];        /* User ID. */
    char c_gid[8];        /* Group ID. */
    char c_nlink[8];      /* Number of hard links. */
    char c_mtime[8];      /* Modification time. */
    char c_filesize[8];   /* File size. */
    char c_devmajor[8];   /* Major dev number. */
    char c_devminor[8];   /* Minor dev number. */
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];   /* Length of filename in bytes. */
    char c_check[8];      /* Checksum. */
};

struct cpio_info {
    unsigned long file_size; 
	unsigned long file_align;
	unsigned long name_size; 
	unsigned long name_align;
    unsigned long offset;

};

unsigned long align(unsigned long  size, unsigned long  multiplier);
void extract_header(struct cpio_newc_header *cpio_addr, struct cpio_info *size_info);
void cpio_list();
void cpio_cat(char *args);
char* find_app_addr(char* target);
int find_app_size(char* target);
void load_app(char *args);

#endif /* _CPIO_H_ */

