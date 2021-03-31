#include "cpio.h"
#include "uart.h"
#include "string.h"
#include "mm.h"
#include "loader.h"

CPIO_HEADER *cpio_base_address = 0x0;
CPIO_index *cpio_index_head = NULL;

CPIO_index *allocate_index_node () {
    u64 size = aligned16(sizeof(CPIO_index));
    CPIO_index *index = (CPIO_index *)startup_malloc(size);
    u64 *ptr = (u64 *)index;
    for (int i = 0; i < size / 8; i++)
        ptr[i] = 0;
    return index;
}

u32 hex2int (char *buf, unsigned int len) {
    unsigned int num = 0;
    for (int i = 0; i < 8; i++) {
        unsigned tmp;
        if ('0' <= buf[i] && buf[i] <= '9')
            tmp = buf[i] - '0';
        else
            tmp = buf[i] - 'A' + 10;

        num += tmp << (4 * (len - i - 1));
    }
    return num;
}

#define is_cpio_header(header) (hex2int((header)->c_magic, 6) == CPIO_MAGIC)

/* return next header */
CPIO_HEADER *parse_cpio_struct (CPIO_HEADER *header, CPIO_index *index) {
    if (!is_cpio_header(header))
        return NULL;
    index->filesize = hex2int(header->c_filesize, 8);
    index->namesize = hex2int(header->c_namesize, 8);
    index->name = &((char *)header)[0x6e];
    index->header = header;

    u64 tmp = (u64)header + 0x6e + index->namesize;
    if (tmp % 4)
        tmp += 4 - tmp % 4;
    index->data = (char *)tmp;

    tmp = (unsigned long)index->data + index->filesize;
    if (tmp % 4)
        tmp += 4 - tmp % 4;
    return (CPIO_HEADER *)tmp;
}

/* initialize cpio index */
void cpio_init () {
    if (is_cpio_header((CPIO_HEADER*) 0x20000000)) {
        cpio_base_address = (CPIO_HEADER *) 0x20000000;
    }
    else if (is_cpio_header((CPIO_HEADER*) 0x8000000)) {
        cpio_base_address = (CPIO_HEADER *) 0x8000000;
    }
    /* no cpio archive */
    else {
        cpio_base_address = (CPIO_HEADER *) 0x0;
        return;
    }

    CPIO_index *prev = allocate_index_node();
    cpio_index_head = prev;
    CPIO_HEADER *header = cpio_base_address;
    boot_info.cpio_addr = (u64)cpio_base_address;
    header = parse_cpio_struct(header, prev);

    while (1) {
        CPIO_index *curr = allocate_index_node();
        header = parse_cpio_struct(header, curr);
        if (!strcmp(curr->name, "TRAILER!!!")) {
            boot_info.cpio_end = (u64)header;
            startup_lock_memory((u64)cpio_base_address, (u64)header);
            break;
        }
        prev->next = curr;
        prev = curr;
    }
    prev->next = NULL;
}

void print_chars (char *addr, unsigned int len) {
    for (int i = 0; i < len; i++) {
        uart_sendc(addr[i]);
    }
}


void header2info (CPIO_HEADER *base, CPIO_INFO *info) {
    info->magic = hex2int(base->c_magic, 6);
    info->ino = hex2int(base->c_ino, 8);
    info->mode = hex2int(base->c_mode, 8);
    info->uid = hex2int(base->c_uid, 8);
    info->gid = hex2int(base->c_gid, 8);
    info->nlink = hex2int(base->c_nlink, 8);
    info->mtime = hex2int(base->c_mtime, 8);
    info->filesize = hex2int(base->c_filesize, 8);
    info->devmajor = hex2int(base->c_devmajor, 8);
    info->devminor = hex2int(base->c_devminor, 8);
    info->rdevmajor = hex2int(base->c_rdevmajor, 8);
    info->rdevminor = hex2int(base->c_rdevminor, 8);
    info->namesize = hex2int(base->c_namesize, 8);
    info->check = hex2int(base->c_check, 8);

    info->current_header = base;
    info->name = &((char *)base)[0x6e];
    unsigned long tmp = (unsigned long)base + 0x6e + info->namesize;
    if (tmp % 4)
        tmp += 4 - tmp % 4;
    info->data = (char *)tmp;

    tmp = (unsigned long)info->data + info->filesize;
    if (tmp % 4)
        tmp += 4 - tmp % 4;
    info->next_header = (CPIO_HEADER *)tmp;
}

void show_cpio_info (CPIO_HEADER *base) {
    uart_send("name: ");
    uart_send((char *)(base + (0x70 - 2)));
    char *ptr = (char *)base;
    uart_send((char *)&ptr[0x6e]);

    uart_send("\r\nmagic: ");
    print_chars(base->c_magic, 6);

    uart_send("\r\nino: ");
    print_chars(base->c_ino, 8);

    uart_send("\r\nmode: ");
    print_chars(base->c_mode, 8);

    uart_send("\r\nuid: ");
    print_chars(base->c_uid, 8);

    uart_send("\r\ngid: ");
    print_chars(base->c_gid, 8);

    uart_send("\r\nnlink: ");
    print_chars(base->c_nlink, 8);

    uart_send("\r\nmtime: ");
    print_chars(base->c_mtime, 8);

    uart_send("\r\nfilesize: ");
    print_chars(base->c_filesize, 8);

    uart_send("\r\ndevmajor: ");
    print_chars(base->c_devmajor, 8);

    uart_send("\r\ndevminor: ");
    print_chars(base->c_devminor, 8);

    uart_send("\r\nrdevmajor: ");
    print_chars(base->c_rdevmajor, 8);

    uart_send("\r\nrdevminor: ");
    print_chars(base->c_rdevminor, 8);

    uart_send("\r\nnamesize: ");
    print_chars(base->c_namesize, 8);

    uart_send("\r\ncheck: ");
    print_chars(base->c_check, 8);
    uart_send("\r\n");
}

CPIO_index * cpio_find_file (char *path) {
    for (CPIO_index *ptr = cpio_index_head; ptr; ptr = ptr->next)
        if (!strcmp(path, ptr->name))
            return ptr;
    return NULL;
}

void cpio_cat_file (char *path) {
    CPIO_index *index = cpio_find_file(path);
    if (index)
        print_chars(index->data, index->filesize);
}

void cpio_cat_interface (char *buffer) {
    char *ptr = &buffer[3];
    for (; *ptr == ' '; ptr++) ;
    cpio_cat_file(ptr);
}

void cpio_show_files () {
    for (CPIO_index *ptr = cpio_index_head; ptr; ptr = ptr->next) {
        uart_send(ptr->name);
        uart_send(" ");
    }
    uart_send("\r\n");
}
