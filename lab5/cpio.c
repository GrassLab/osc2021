#include "include/uart.h"
#include "include/cpio.h"
#include "include/string.h"
#include "include/util.h"
#include "include/allocator.h"
#include "include/interrupt.h"

#define CPIO_ADDR   0x8000000
#define CPIO_HEADER_SIZE    110

int initramfs_traverse(int (*it)(cpio_newc_header *cpio_head, int namesize, int filesize, void *data), void *data) {
    int namesize, filesize, rc = 0;
    cpio_newc_header *cpio_head = (cpio_newc_header *)CPIO_ADDR;
    
    while (!strlcmp(cpio_head->c_mode, "00000000", 8) && !rc) {
        namesize = strl2int(cpio_head->c_namesize, 8);
        filesize = strl2int(cpio_head->c_filesize, 8);

        rc = it(cpio_head, namesize, filesize, data);

        cpio_head = (cpio_newc_header *)(align(align((unsigned long)(cpio_head) + CPIO_HEADER_SIZE + namesize, 4) + filesize, 4));
    }
    
    return rc;
}

int list_action(cpio_newc_header *cpio_head, int namesize, int filesize, void *data) {
    if (strlcmp(cpio_head->c_mode, "000081B4", 8)) {
        uart_put_str("[txt]\t");
        uart_put_str((char *)cpio_head + CPIO_HEADER_SIZE);
        uart_put_str("\n");
    }
    else if (strlcmp(cpio_head->c_mode, "000041FD", 8)) {
        uart_put_str("[dir]\t");
        uart_put_str((char *)cpio_head + CPIO_HEADER_SIZE);
        uart_put_str("\n");
    }
    else if (strlcmp(cpio_head->c_mode, "000081FD", 8)) {
        uart_put_str("[exe]\t");
        uart_put_str((char *)cpio_head + CPIO_HEADER_SIZE);
        uart_put_str("\n");
    }
    return 0;
}

void list() {
    initramfs_traverse(list_action, NULL);
}

int cat_action(cpio_newc_header *cpio_head, int namesize, int filesize, void *data) {
    char *pathname = (char *)data;
    if (strcmp((char *)cpio_head + CPIO_HEADER_SIZE, pathname)) {
        char *file_content = (char *)cpio_head + align(CPIO_HEADER_SIZE + namesize, 4);
        for (int i = 0; i < filesize ; i++) {
            uart_send(file_content[i]);
        }
        uart_send('\n');
        return 1;
    }
    return 0;
}

void cat(char *pathname) {
    int rc;
    rc = initramfs_traverse(cat_action, (void *)pathname);
    if (!rc)
        uart_put_str("The file is not found.\n");
}

int run_action(cpio_newc_header *cpio_head, int namesize, int filesize, void *data) {
    char *pathname = (char *)data;
    if (strcmp((char *)cpio_head + CPIO_HEADER_SIZE, pathname) && strlcmp(cpio_head->c_mode, "000081ED", 8)) {
        char *file_content = (char *)cpio_head + align(CPIO_HEADER_SIZE + namesize, 4);
        char *load_addr = (char *)get_memory(filesize);
        char *sp_frame_start = (char *)get_memory(FRAME_SIZE);
        unsigned long sp_addr = (unsigned long)sp_frame_start + FRAME_SIZE;
        
        for (int i = 0; i < filesize ; i++) {
            load_addr[i] = file_content[i];
        }

        asm volatile (
            "mov x10, 0x3c0\n\t"  // mov x10, 0x3c0 (set D, A, I, F to 1)
            "msr spsr_el1, x10\n\t"
            "msr elr_el1, %[load_addr]\n\t"
            "msr sp_el0, %[sp_addr]\n\t"
            "eret\n\t"
            :
            : [load_addr] "r" (load_addr), [sp_addr] "r" (sp_addr)
            :
        );
        
        free_memory(filesize, (unsigned long)load_addr);

        return 1;
    }
    return 0;
}

void run(char *pathname) {
    int rc;
    rc = initramfs_traverse(run_action, (void *)pathname);
    if (!rc)
        uart_put_str("The file is not found.\n");
}

int copy_file_action(cpio_newc_header *cpio_head, int namesize, int filesize, void *data) {
    char *pathname = (char *)((unsigned long *)data)[0];
    unsigned long *start_addr = (unsigned long *)((unsigned long *)data)[1];
    int *size = (int *)((unsigned long *)data)[2];
    *size = filesize;
    
    if (strcmp((char *)cpio_head + CPIO_HEADER_SIZE, pathname) && strlcmp(cpio_head->c_mode, "000081ED", 8)) {
        char *file_content = (char *)cpio_head + align(CPIO_HEADER_SIZE + namesize, 4);
        *start_addr = get_memory(filesize);
        char *load_addr = (char *)(*start_addr);
        for (int i = 0; i < filesize ; i++) {
            load_addr[i] = file_content[i];
        }
        return 1;
    }
    return 0;
}

void copy_file(const char *pathname, unsigned long *start_addr, int *filesize) {
    unsigned long data[3];
    data[0] = (unsigned long)pathname;
    data[1] = (unsigned long)start_addr;
    data[2] = (unsigned long)filesize;
    int rc;
    rc = initramfs_traverse(copy_file_action, (void *)data);
    if (!rc)
        uart_put_str("The file is not found.\n");
}