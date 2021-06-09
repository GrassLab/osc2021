#ifndef CPIO_H
#include "data_type.h"
#define CPIO_H
#define CPIO_MAGIC 0x070701

#define is_cpio_file(info) (info.mode & 0x8000)
#define is_cpio_dir(info) (info.mode & 0x4000)

typedef struct cpio_newc_header {
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
} CPIO_HEADER;

typedef struct cpio_newc_info {
    u32 magic;
    u32 ino;
    u32 mode;
    u32 uid;
    u32 gid;
    u32 nlink;
    u32 mtime;
    u32 filesize;
    u32 devmajor;
    u32 devminor;
    u32 rdevmajor;
    u32 rdevminor;
    u32 namesize;
    u32 check;
    char *name, *data;
    CPIO_HEADER *next_header;
    CPIO_HEADER *current_header;
    struct cpio_newc_info *next;
} CPIO_INFO;

typedef struct cpio_newc_index {
    u32 namesize;
    u32 filesize;
    char *name, *data;
    CPIO_HEADER *header;
    struct cpio_newc_index *next;
} CPIO_index;

extern CPIO_HEADER *cpio_base_address;


void cpio_init ();

void show_cpio_info (CPIO_HEADER *base);
void cpio_show_files ();
void cpio_cat_file (char *path);
void cpio_cat_interface (char *buffer);
int cpio_load_file_interface (char *buffer);
void *cpio_load_file (char *path);
void cpio_vfs_mount ();

#endif
