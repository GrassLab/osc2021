#ifndef CPIOH
#define CPIOH

// https://www.freebsd.org/cgi/man.cgi?query=cpio&sektion=5

#define	CPIO_MAGIC		"070701"		// cpio magic number
#define	CPIO_TRAILER	"TRAILER!!!"	// cpio end string 

struct cpio_newc_header 
{
  char c_magic[6];			// magic number "070701"
  char c_ino[8];			// "i-node" number
  char c_mode[8];			// file mode
  char c_uid[8];			// user ID
  char c_gid[8];			// group ID
  char c_nlink[8];			// number of hard links
  char c_mtime[8];			// modification time
  char c_filesize[8];		// file size
  char c_devmajor[8];		// major device number
  char c_devminor[8];		// minor device number
  char c_rdevmajor[8];		// only valid for chr and blk special files
  char c_rdevminor[8];		// only valid for chr and blk special files
  char c_namesize[8];		// length of name in bytes ( include '\0')
  char c_check[8];			// checksum
};

unsigned long long align_to_4(unsigned long long size); // align to 4 byte
void cpio_list(); // list rootfs file
char* cpio_content(char* name); // get file content
void cpio_run_user_program(char* name, int enable_timer);
int cpio_load_user_program_and_get_size(char* name, unsigned long long loadAddr);

#endif
