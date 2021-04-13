# include "uart.h"
#define INITRAMFS_ADDR (cpio_newc_header *)0x8000000
//#define INITRAMFS_ADDR (cpio_newc_header *)0x20000000

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
  char c_check[8];       // 0 OR CRC of bytes of FILE data
} cpio_newc_header;

inline unsigned long align4(unsigned long n);
void list();
void show_file(char *file_name);
void show_file(char *file_name);
void exec_app(char *file_name);
