#ifndef __CPIO_H
#define __CPIO_H

#include <stddef.h>
#include "miniuart.h"
#include "string.h"
#include "dtb.h"
#include "printf.h"
#include "allocator.h"
#include "elf.h"

#define CPIO_MAGIC_NUM "070701"
#define CPIO_MODE_FILE 0100000
#define CPIO_MODE_DIR 0040000
#define ROUNDUP_MUL4(num) ((num + 0x3) & ~0x3)

typedef struct {
    char c_magic[6];        // magic cookie
    char c_ino[8];		    // inode number
	char c_mode[8];		    // file type/access
	char c_uid[8];		    // owners uid
	char c_gid[8];		    // owners gid
	char c_nlink[8];        // # of links at archive creation
	char c_mtime[8];	    // modification time
	char c_filesize[8];	    // length of file in bytes
	char c_devmajor[8];	    // block/char major #
	char c_devminor[8];	    // block/char minor #
	char c_rdevmajor[8];    // special file major #
	char c_rdevminor[8];    // special file minor #
	char c_namesize[8];	    // length of pathname
	char c_check[8];	    // 0 OR CRC of bytes of FILE data
} cpio_newc_header;

void ls();
void cat(const char *find_filename);
void load(const char *find_filename);

#endif
