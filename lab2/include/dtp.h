#ifndef _DTP_H_
#define _DTP_H_

// #define INITRAMFS_BASE 0x20000000 // rpi3
#define INITRAMFS_BASE 0x8000000 // QEMU

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
};

struct dtn {
    char *name;
    char *compatible;
};
int do_dtp(int (*_probe_func)(struct dtn *node));

#endif /*_DTP_H_ */