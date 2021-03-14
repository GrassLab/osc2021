#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_RSTS ((volatile unsigned int *)(0x3F100020))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))
#define SYS_CMD_NUM 6
#define CPIO_ADDR ((char*)0x20000000) //QEMU(0x8000000)
struct cpio_size_info{
    unsigned long long int file_size, file_padding, name_size, name_padding;
    unsigned long long int offset;
};
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
void system_command(char*);
void sys_help(char*);
void sys_hello(char*);
void sys_reboot(char*);
void reset(int );
void cancel_reset();
void sys_list(char*);
void sys_cat(char*);
void extract_header(struct cpio_newc_header *, struct cpio_size_info *);
unsigned long long int hex2int(char*, int);
void *__memset(void*, int, int);

#endif
