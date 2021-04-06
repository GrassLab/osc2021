#ifndef CPIO_H
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
    unsigned int magic;
    unsigned int ino;
    unsigned int mode;
    unsigned int uid;
    unsigned int gid;
    unsigned int nlink;
    unsigned int mtime;
    unsigned int filesize;
    unsigned int devmajor;
    unsigned int devminor;
    unsigned int rdevmajor;
    unsigned int rdevminor;
    unsigned int namesize;
    unsigned int check;
    char *name, *data;
    CPIO_HEADER *next_header;
    CPIO_HEADER *current_header;
} CPIO_INFO;

extern CPIO_HEADER *cpio_base_address;
void show_cpio_info (CPIO_HEADER *base);
void cpio_show_files ();
void cpio_cat_file (char *path);
void cpio_cat_interface (char *buffer);

#endif
