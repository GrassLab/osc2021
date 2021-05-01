#include "elf.h"

static uint32_t elf_is_valid(void *p){
    Elf32_Ehdr *h = p;
    if(h->e_ident[EI_MAG0] != 0x7f){
        return 0;
    }
    if(h->e_ident[EI_MAG1] != 'E'){
        return 0;
    }
    if(h->e_ident[EI_MAG2] != 'L'){
        return 0;
    }
    if(h->e_ident[EI_MAG3] != 'F'){
        return 0;
    }
    return 1;
}

int64_t elf_start_offset(void *p){
    if(elf_is_valid(p)){
        Elf32_Ehdr *h = p;
        Elf32_Shdr *sh = p + h->e_shoff;
        return h->e_entry - sh->sh_offset;
    }
    return -1;
}
