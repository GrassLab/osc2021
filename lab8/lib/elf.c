#include <elf.h>
#include <string.h>

size_t elf_entry(void *elf) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;
    Elf64_Phdr *phdr = (Elf64_Phdr *)((size_t)elf + ehdr->e_phoff);

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            return ehdr->e_entry;
        }
    }

    return 0;
}

int check_elf(void *elf) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf;
    if (strncmp((char *)ehdr->e_ident, ELFMAG, SELFMAG)) {
        return 0;
    }

    return 1;
}
