#include "cpio.h"

void ls(){
    cpio_newc_header *blk = INITRAMFS_ADDR;
    if(strncmp(blk->c_magic, CPIO_MAGIC_NUM, sizeof(blk->c_magic))){
        miniuart_send_S("Error: Invalid cpio's New ASCII Format!!!" NEW_LINE);
        return;
    }

    while(1){
        size_t namesize = xatou32(blk->c_namesize, sizeof(blk->c_namesize));
        char *name = ((char *)blk + sizeof(cpio_newc_header));
        if(!strcmp("TRAILER!!!", name)){
            break;
        }

        uint32_t mode = xatou32(blk->c_mode, sizeof(blk->c_mode));
        if(mode & CPIO_MODE_FILE){
            miniuart_send_nS(name, namesize);
            miniuart_send_S("  ");
        }

        size_t filesize = xatou32(blk->c_filesize, sizeof(blk->c_filesize));
        char *file = (char *)ROUNDUP_MUL4((uint64_t)name + namesize);
        blk = (cpio_newc_header *)ROUNDUP_MUL4((uint64_t)file + filesize);
    }
    miniuart_send_S(NEW_LINE);
}

void cat(const char *find_filename){
    if(!find_filename){
        miniuart_send_S("Usage: cat <filename>" NEW_LINE);
        return;
    }

    cpio_newc_header *blk = INITRAMFS_ADDR;
    if(strncmp(blk->c_magic, CPIO_MAGIC_NUM, sizeof(blk->c_magic))){
        miniuart_send_S("Invalid cpio's New ASCII Format!!!" NEW_LINE);
        return;
    }

    while(1){
        size_t namesize = xatou32(blk->c_namesize, sizeof(blk->c_namesize));
        char *name = ((char *)blk + sizeof(cpio_newc_header));
        if(!strcmp("TRAILER!!!", name)){
            break;
        }

        size_t filesize = xatou32(blk->c_filesize, sizeof(blk->c_filesize));
        char *file = (char *)ROUNDUP_MUL4((uint64_t)name + namesize);

        uint32_t mode = xatou32(blk->c_mode, sizeof(blk->c_mode));
        if(mode & CPIO_MODE_FILE){
            if(!strcmp(find_filename, name)){
                if(filesize){
                    miniuart_send_nS(file, filesize);
                    miniuart_send_C('\r');
                }else{
                    miniuart_send_S(NEW_LINE);
                }
                return;
            }
        }

        blk = (cpio_newc_header *)ROUNDUP_MUL4((uint64_t)file + filesize);
    }
    miniuart_send_S("cat: ");
    miniuart_send_S(find_filename);
    miniuart_send_S(": No such file" NEW_LINE);
}
