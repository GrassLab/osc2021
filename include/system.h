#ifndef __SYSTEM_H__
#define __SYSTEM_H__
#include "utils.h"
#include "buddy.h"
#include "dynamic_allocator.h"
#include "uart.h"
#include "entry.h"
#include "thread.h"
#include "timer.h"
#include "vfs.h"
#include <stddef.h>

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_RSTS ((volatile unsigned int *)(0x3F100020))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))

#define CPIO_ADDR ((char *)0x20000000) //QEMU(0x8000000)0x20000000
#define KB 0x400
#define MB 0x100000

#define nullptr ((void *)0)

extern int DEBUG;
extern void core_timer_enable(void);

struct cpio_size_info
{
	unsigned long long int file_size, file_padding, name_size, name_padding;
	unsigned long long int offset;
};
struct cpio_newc_header
{
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
void sys_init_vfs();
void system_command(char *);
void sys_help(char *);
void sys_hello(char *);
void sys_reboot(char *);
void reset(int);
void cancel_reset();
void sys_list(char *);
void sys_cat(char *);
void sys_setTimeout(char *);
void sys_disable_timer(char *);
void sys_load_user_program(char *);
void sys_clear(char *);
void extract_header(struct cpio_newc_header *, struct cpio_size_info *);
struct cpio_newc_header *find_cpio_entry(char *file_name);
unsigned long long int hex2int(char *, int);
void *__memset(void *, int, int);
uint32_t sys_get32bits(char *);
uint64_t sys_get64bits(char *);
unsigned long long int need_padding(unsigned long long int size, unsigned long long int multiplier);
void swap(int *, int *);
void *malloc(int);
void *kmalloc(int);
void free(void *);
void __lab3(char *);
void __lab5(char *);
void __lab5_2(char *);
void __lab6(char *);
int sys_open(const char *pathname, int flags);
void sys_close(int fd);
int sys_write(int fd, const void *buf, size_t len);
int sys_read(int fd, void *buf, size_t len);
int sys_register_fd(struct file *file);
int sys_list_vfs(int fd, void *buf, int id);
#endif
