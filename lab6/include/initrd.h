/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef __INITRD__
#define __INITRD__

/* cpio hpodc format */
typedef struct {
    char magic[6]; //070701
    char ino[8];  //inodes number from disk
    char mode[8]; //specifies both the regular permissions and file type
    char uid[8]; //userid
    char gid[8]; //groupid
    char nlink[8];  //number of links to this file
    char mtime[8];  //modification time
    char filesize[8];
    char devmajor[8];
    char devminor[8];
    char rdevmajor[8];
    char rdevminor[8];
    char namesize[8];
    char check[8]; //always set to 0 by writers and ignored by reader
} __attribute__((packed)) cpio_t; // tell compiler don't do the alignment in order to save memory space
void cpio();
void loadprog();
unsigned long loadprogWithArgv(char *path, unsigned long a_addr, char **argv);
char* fnameGet(cpio_t* cpio_addr);
char* fdataGet(cpio_t* cpio_addr, unsigned long *fsize);
int fmodeGet(cpio_t* cpio_addr);
cpio_t* nextfile(cpio_t* cpio_addr);
#endif
