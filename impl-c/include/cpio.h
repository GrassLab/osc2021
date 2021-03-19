#pragma once

#include <stddef.h>
#include <stdint.h>

// CPIO manual page
// https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5
#define RAMFS_ADDR 0x8000000

#define CPIO_HEADER_MAGIC "070701"
#define CPIO_FOOTER_MAGIC "TRAILER!!!"

// Header of cpio format: `newc`
typedef struct cpioNewcHeader {
  char magic[6];
  char ino[8];
  char mode[8];
  char uid[8];
  char gid[8];
  char nlink[8];
  char mtime[8];
  char filesize[8];
  char devmajor[8];
  char devminor[8];
  char rdevmajor[8];
  char rdevminor[8];
  char namesize[8];
  char check[8];
} CpioNewcHeader;

// The Overview information of a cpio archive file
typedef struct cpioSummaryInfo {
  unsigned int numFiles;
  unsigned int mxPathSize;
} CpioSummaryInfo;

// Get summary information inside a CPIO archive
int cpioInfo(void *archive, CpioSummaryInfo *info);

// Print file names inside a CPIO archive (using uart)
int cpioLs(void *archive);

// Get file by return it's content and the size in memory.
void *cpioGetFile(void *archive, const char *name, unsigned long *size);
