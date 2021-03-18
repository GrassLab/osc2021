#include "uart.h"

int memcmp(void* s1, void* s2, int n)
{
    unsigned char *a = s1, *b = s2;
    while (n--) {
        if (*a != *b) {
            return *a - *b;
        }
        a++;
        b++;
    }
    return 0;
}

/* cpio hpodc format */
typedef struct {
    char magic[6]; /* Magic header '070707'. */
    char ino[8]; /* "i-node" number. */
    char mode[8]; /* Permisions. */
    char uid[8]; /* User ID. */
    char gid[8]; /* Group ID. */
    char nlink[8]; /* Number of hard links. */
    char mtime[8]; /* Modification time. */
    char filesize[8]; /* File size. */
    char devmajor[8]; /* device number. */
    char devminor[8]; /* device number. */
    char rdevmajor[8]; /* device major/minor number. */
    char rdevminor[8]; /* device major/minor number. */
    char namesize[8]; /* Length of filename in bytes. */
    char check[8];
} __attribute__((packed)) cpio_t;

/**
 * Helper function to convert ASCII octal number into binary
 * s string
 * n number of digits
 */
int hex2dec(char* s, int n)
{
    int r = 0;
    while (n--) {
        r *= 16;
        int dif = *s++ - '0';
        if (dif < 10) {
            r += dif;
        } else {
            r += dif - 7;
        }
    }
    return r;
}

/**
 * List the contents of an archive
 */
void initrd_list(char* buf)
{
    // iterate on archive's contents
    // if it's a cpio archive. Cpio also has a trailer entry
    while (!memcmp(buf, "070701", 6) && memcmp(buf + sizeof(cpio_t), "TRAILER!!", 9)) {
        cpio_t* header = (cpio_t*)buf;
        int ns = hex2dec(header->namesize, 8);
        int fs = hex2dec(header->filesize, 8);
        uart_puts("pathname: ");
        uart_Wputs(buf + sizeof(cpio_t), ns);

        if ((sizeof(cpio_t) + ns) % 4) {
            buf += (((sizeof(cpio_t) + ns) / 4) + 1) * 4;
        } else {
            buf += sizeof(cpio_t) + ns;
        }

        uart_puts("file content: ");
        uart_Wputs(buf, fs);

        if ((fs) % 4) {
            buf += ((fs / 4) + 1) * 4;
        } else {
            buf += fs;
        }
    }
}