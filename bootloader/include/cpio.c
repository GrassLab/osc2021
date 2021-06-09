#include "cpio.h"
#include "uart.h"
#include "string.h"

#ifdef M_RASPI3
CPIO_HEADER *cpio_base_address = (CPIO_HEADER *) 0x20000000;
#else
CPIO_HEADER *cpio_base_address = (CPIO_HEADER *) 0x8000000;
#endif

void print_chars (char *addr, unsigned int len) {
    for (int i = 0; i < len; i++) {
        uart_sendc(addr[i]);
    }
}

unsigned hex2int (char *buf, unsigned int len) {
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

CPIO_HEADER * cpio_find_file (char *path) {
    CPIO_HEADER *cpioh = cpio_base_address;
    CPIO_INFO cpio_info;

    while (1) {
        header2info(cpioh, &cpio_info);
        if (cpio_info.magic != CPIO_MAGIC)
            break;
        if (!strcmp(path, cpio_info.name))
            return cpioh;

        cpioh = cpio_info.next_header;
    }

    return 0;
}

void cpio_cat_file (char *path) {
    CPIO_HEADER *h = cpio_find_file(path);
    if (!h) {
        uart_send("No such file: ");
        uart_send(path);
        uart_send("\r\n");
        return;
    }

    CPIO_INFO cpio_info;
    header2info(h, &cpio_info);

    if (!is_cpio_file(cpio_info)) {
        uart_send(path);
        uart_send(" is not a file");
        uart_send("\r\n");
        return;
    }

    print_chars(cpio_info.data, cpio_info.filesize);
}

void cpio_cat_interface (char *buffer) {
    char *ptr = &buffer[3];
    for (; *ptr == ' '; ptr++) ;
    cpio_cat_file(ptr);
}

void cpio_show_files () {
    CPIO_HEADER *cpioh = cpio_base_address;
    CPIO_INFO cpio_info;

    while (1) {
        header2info(cpioh, &cpio_info);
        if (cpio_info.magic != CPIO_MAGIC)
            break;
        cpioh = cpio_info.next_header;

        if (is_cpio_file(cpio_info) || is_cpio_dir(cpio_info))
            uart_send(cpio_info.name);
        uart_send(" ");
    }
    uart_send("\r\n");
}

