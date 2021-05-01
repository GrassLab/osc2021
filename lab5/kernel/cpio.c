#include "cpio.h"

extern uint64_t INITRAMFS_ADDR;
extern uint64_t INITRAMFS_ADDR_END;
extern void from_el1_to_el0(void (*main)(), void (*load_t)(), void *sp);
extern void load_template(void (*main)(), uint64_t *sp);

void cpio_init(){
    const uint32_t *prop = dtb_get_node_prop_addr("chosen", NULL);
    if(!prop){
        miniuart_send_S("Error: dtb has no 'chosen' node!!!" NEW_LINE);
        return;
    }
    uint32_t value;
    int32_t n = dtb_get_prop_value("linux,initrd-start", prop, &value);
    if(n == -1){
        miniuart_send_S("Error: 'chosen' node has no 'linux,initrd-start' property!!!" NEW_LINE);
        return;
    }
    if(n != 4){
        //printf("Error: 'linux,initrd-start' property gives size of %d!!!" NEW_LINE, n);
        miniuart_send_S("Error: 'linux,initrd-start' property gives size of not 4!!!" NEW_LINE);
        return;
    }
    INITRAMFS_ADDR = (uint64_t)bswap32(value);

    n = dtb_get_prop_value("linux,initrd-end", prop, &value);
    if(n == -1){
        miniuart_send_S("Error: 'chosen' node has no 'linux,initrd-end' property!!!" NEW_LINE);
        return;
    }
    if(n != 4){
        //printf("Error: 'linux,initrd-end' property gives size of %d!!!" NEW_LINE, n);
        miniuart_send_S("Error: 'linux,initrd-end' property gives size of not 4!!!" NEW_LINE);
        return;
    }
    INITRAMFS_ADDR_END = (uint64_t)bswap32(value);
}

void ls(){
    if(INITRAMFS_ADDR == 0){
        miniuart_send_S("Error: initramfs not loaded!!!" NEW_LINE);
        return;
    }

    cpio_newc_header *blk = (cpio_newc_header *)INITRAMFS_ADDR;
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

    if(INITRAMFS_ADDR == 0){
        miniuart_send_S("Error: initramfs not loaded!!!" NEW_LINE);
        return;
    }

    cpio_newc_header *blk = (cpio_newc_header *)INITRAMFS_ADDR;
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

static void *load_setup_argv(char *sp, char *const argv[]){
    if(!argv){
        sp -= 16;
        uint64_t *sp_u64 = (uint64_t *)sp;
        sp_u64[0] = 0;
        sp_u64[1] = 0;
        return sp;
    }

    uint64_t argc = 0;
    char *arg = argv[argc];
    while(arg){
        argc++;
        arg = argv[argc];
    }
    char *argv_temp[argc];
    for(size_t i=1; i<=argc; i++){
        size_t len = strlen(argv[argc - i]) + 1;
        sp -= ROUNDUP_MUL8(len);
        for(size_t j=0; j<len; j++){
            sp[j] = argv[argc - i][j];
        }
        argv_temp[argc - i] = sp;
    }
    sp -= ROUNDUP_MUL16((argc + 3) * 8);
    uint64_t *sp_u64 = (uint64_t *)sp;
    sp_u64[0] = argc;
    sp_u64[1] = (uint64_t)&sp_u64[2];
    for(size_t i=0; i<argc; i++){
        sp_u64[i+2] = (uint64_t)argv_temp[i];
    }
    sp_u64[argc+2] = 0;
    return sp;
}

static void load(const char *find_filename, char *const argv[], Load_return *ret){
    ret->npage = 0;

    if(!find_filename){
        miniuart_send_S("Usage: load <filename>" NEW_LINE);
        return;
    }

    if(INITRAMFS_ADDR == 0){
        miniuart_send_S("Error: initramfs not loaded!!!" NEW_LINE);
        return;
    }

    cpio_newc_header *blk = (cpio_newc_header *)INITRAMFS_ADDR;
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

        size_t filesize = xatou32(blk->c_filesize, sizeof(blk->c_filesize));
        char *file = (char *)ROUNDUP_MUL4((uint64_t)name + namesize);

        uint32_t mode = xatou32(blk->c_mode, sizeof(blk->c_mode));
        if(mode & CPIO_MODE_FILE){
            if(!strcmp(find_filename, name)){
                if(filesize){
                    int64_t start_offset = elf_start_offset(file);
                    if(start_offset >= 0){
                        ret->npage = (((filesize + 0xFFFU) & ~(0xFFFU)) / PAGE_SIZE) + 1;
                        char *p = palloc(ret->npage);
                        if(p){
                            for(size_t i=0; i<filesize; i++){
                                p[i] = file[i];
                            }
                            ret->user_stack = p;
                            ret->sp = load_setup_argv((char *)p + (ret->npage * PAGE_SIZE), argv);
                            ret->pc = (void *)p + start_offset;
                        }else{
                            ret->npage = 0;
                            miniuart_send_S("Error: Load failed!!!" NEW_LINE);
                        }
                    }else{
                        miniuart_send_S("Error: Invalid ELF format!!!" NEW_LINE);
                    }
                }else{
                    miniuart_send_S(NEW_LINE);
                }
                return;
            }
        }

        blk = (cpio_newc_header *)ROUNDUP_MUL4((uint64_t)file + filesize);
    }
    miniuart_send_S("load: ");
    miniuart_send_S(find_filename);
    miniuart_send_S(": No such file" NEW_LINE);
}

int64_t exec(const char* name, char *const argv[]){
    Load_return ret;
    load(name, argv, &ret);
    if(ret.npage == 0){
        return -1;
    }

    int64_t pid = fork();
    if(pid < 0){
        free(ret.user_stack);
        return -1;
    }

    if(pid == 0){
        Thread *child = current_thread();
        child->user_stack_npage = ret.npage;
        child->user_stack = ret.user_stack;
        from_el1_to_el0(ret.pc, load_template, ret.sp);
    }

    return 0;
}
