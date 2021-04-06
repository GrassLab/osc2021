#include "../include/cpio.h"
#include "../include/string.h"
#include "../include/uart.h"

#define NULL ((void *)0)

struct cpio_header_info {
    const char *filename;
    unsigned long filesize;
    void *data;
    struct cpio_header *next;
};

/* Align 'n' up to the value 'align', which must be a power of two. */
static unsigned long align_up(unsigned long n, unsigned long align)
{
    return (n + align - 1) & (~(align - 1));
}

/* Parse an ASCII hex string into an integer. */
static unsigned long parse_hex_str(char *s, unsigned int max_len)
{
    unsigned long r = 0;
    unsigned long i;

    for (i = 0; i < max_len; i++) {
        r *= 16;
        if (s[i] >= '0' && s[i] <= '9') {
            r += s[i] - '0';
        }  else if (s[i] >= 'a' && s[i] <= 'f') {
            r += s[i] - 'a' + 10;
        }  else if (s[i] >= 'A' && s[i] <= 'F') {
            r += s[i] - 'A' + 10;
        } else {
            return r;
        }
        continue;
    }
    return r;
}

static int cpio_strncmp(const char *a, const char *b, unsigned long n)
{
    unsigned long i;
    for (i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
        if (a[i] == 0) {
            return 0;
        }
    }
    return 0;
}

static char* cpio_strcpy(char *to, const char *from) {
    char *save = to;
    while (*from != 0) {
        *to = *from;
        to++;
        from++;
    }
    return save;
}

static unsigned int cpio_strlen(const char *str) {
    const char *s;
    for (s = str; *s; ++s) {}
    return (s - str);
}

/* Calculate the remaining length in a CPIO file after reading a header. */
static unsigned long cpio_len_next(unsigned long len, void *prev, void *next) {
    unsigned long diff = (unsigned long) (next - prev);
    if (len < diff) {
        return 0;
    }
    return len;
}

int cpio_parse_header(struct cpio_header *archive, struct cpio_header_info *info)
{
    uart_puts("");
    const char *filename;
    unsigned long filesize;
    unsigned long filename_length;
    void *data;
    struct cpio_header *next;

     /* Ensure magic header exists. */
    if (cpio_strncmp(archive->c_magic, CPIO_HEADER_MAGIC, sizeof(archive->c_magic)) != 0) {
        // uart_puts("1\n");
        return -1;
    }

    /* Get filename and file size. */
    filesize = parse_hex_str(archive->c_filesize, sizeof(archive->c_filesize));
    filename_length = parse_hex_str(archive->c_namesize, sizeof(archive->c_namesize));
    filename = (char *) archive + sizeof(struct cpio_header);

    /* Ensure filename is terminated */
    if (filename[filename_length - 1] != 0) {
        // uart_puts("2\n");
        return -1;
    }

    /* Ensure filename is not the trailer indicating EOF. */
    if (filename_length >= sizeof(CPIO_FOOTER_MAGIC) && cpio_strncmp(filename,
                CPIO_FOOTER_MAGIC, sizeof(CPIO_FOOTER_MAGIC)) == 0) {
        // uart_puts("3\n");
        return 1;
    }

    /* Find offset to data. */
    data = (void *) align_up((unsigned long) archive + sizeof(struct cpio_header) +
            filename_length, CPIO_ALIGNMENT);
    next = (struct cpio_header *) (data + filesize);

    if (info) {
        info->filename = filename;
        info->filesize = filesize;
        info->data = data;
        info->next = next;
    }
    // uart_puts("----\n");
    // uart_puts("filename\n");
    // uart_puts(info->filename);
    // uart_puts("\n");
    // uart_puts("filesize\n");
    // uart_puts(info->filesize);
    // uart_puts("\n");
    // uart_puts("data\n");
    // uart_puts(data);
    // uart_puts("\n");
    // uart_puts("next\n");
    // uart_puts(next);
    // uart_puts("\n");
    // uart_puts("----\n");
    return 0;
}

int cpio_info(void *archive, struct cpio_info *info) {

    struct cpio_header *header;
    unsigned long current_path_sz;
    struct cpio_header_info header_info;

    if (info == NULL) return 1;
    info->file_count = 0;
    info->max_path_sz = 0;

    header = archive;

    while (1) {

        int error;
        error = cpio_parse_header(header, &header_info);

        if (error == -1) {
            // uart_puts("?\n");
            return error;
        } else if (error == 1) {
            /* EOF */
            // uart_puts("??\n");
            return 0;
        }
        else{
            // uart_puts("???\n");
        }
       // uart_puts(header_info.filename);
        info->file_count++;
        header = header_info.next;

        // Check if this is the maximum file path size.
        current_path_sz = cpio_strlen(header_info.filename);
        if (current_path_sz > info->max_path_sz) {
            info->max_path_sz = current_path_sz;
        }
    }

    return 0;
}

void cpio_ls(void *archive, char buf[100][100], unsigned long buf_len) {

    struct cpio_header_info header_info;
    struct cpio_header *header, *next;
    void *result;
    unsigned long i, size;

    header = archive;

    for (i = 0; i < buf_len; i++) {
        int error = cpio_parse_header(header, &header_info);
        // Break on an error or nothing left to read.
        if (error) break;
        cpio_strcpy(buf[i], header_info.filename);
        header = header_info.next;
    }
}

void cpio_get_file(void *archive, char name[], char content[])
{
    struct cpio_header *header = archive;
    struct cpio_header_info header_info;

    /* Find n'th entry. */
    while (1) {
        
        cpio_parse_header(header, &header_info);
        
        if (cpio_strncmp(header_info.filename, name, strlen(name)) == 0) 
            break;
        
        header = header_info.next;
    }

    // uart_puts(name);
    // uart_send('\n');
    // uart_send('\r');
    // uart_puts(header_info.data);
    // uart_send('\n');
    // uart_send('\r');
    cpio_strcpy(content, header_info.data);
    content[header_info.filesize]='\0';

}