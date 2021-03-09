#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
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

int hex_string_to_int(char *hex_str, int len)
{
	int num, base;
	char *ch;

	num = 0;
	base = 1;
	ch = hex_str + len - 1;
	while(ch >= hex_str) {
		if (*ch >= 'A')
			num += (base*(10 + *ch - 'A'));
		else
			num += (base*(*ch - '0'));
		base *= 16;
		ch--;
	}
	return num;
}

char *align_upper(char *addr, int alignment)
{
	char *res;
	int r = (unsigned long)addr % alignment;
	res = r ? addr + alignment - r : addr;
	return res;
}

int strcmp(char *str1, char *str2)
{
    int i;

    i = 0;
    while (str1[i] != '\0') {
        if (str1[i] != str2[i])
            return 1;
        i++;
    }
    if (str2[i] != '\0')
        return 1;
    return 0;
}

int main(int argc, char const *argv[])
{
	char buf[120];
	int cnt;
	struct stat st;
	struct cpio_newc_header *ent;
	int filesize, namesize, remainder;
	char *name_start, *data_start;
	int fd;
	if ((fd = open("initramfs.cpio", O_RDONLY)) == -1)
		perror("fd");
	fstat(fd, &st);
	int size = st.st_size;

	printf("size=%d, fd=%d\n", size, fd);
	char *pyc_addr = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (pyc_addr == MAP_FAILED)
		perror("mmap");

	ent = (struct cpio_newc_header*)pyc_addr;
	while (1)
	{
		namesize = hex_string_to_int(ent->c_namesize, 8);
		filesize = hex_string_to_int(ent->c_filesize, 8);
		name_start = ((char *)ent) + sizeof(struct cpio_newc_header);
		data_start = align_upper(name_start + namesize, 4);
		printf("name=%s\n", name_start);
		printf("data=%s\n", data_start);
		ent = (struct cpio_newc_header*)align_upper(data_start + filesize, 4);

		if (!strcmp(name_start, "TRAILER!!!"))
			break;
	}
	printf("Bye\n");

	// struct cpio_newc_header *ent = malloc(sizeof(struct cpio_newc_header));

	// cnt = read(fd, ent, sizeof(struct cpio_newc_header));
	// printf("fd=%d, cnt=%d\n", fd, cnt);
	// for (int i = 0; i < 8; ++i)
	// {
	// 	printf("%c", *(ent->c_namesize+i));
	// }

	// struct cpio_newc_header ent;
	// cnt = read(fd, &ent, sizeof(struct cpio_newc_header));
	// printf("fd=%d, cnt=%d\n", fd, cnt);
	// for (int i = 0; i < 8; ++i)
	// {
	// 	printf("%c", *(ent.c_namesize+i));
	// }
	return 0;
}