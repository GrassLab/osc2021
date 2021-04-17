#ifndef __ELF_H
#define __ELF_H

#include <stdint.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

#define EI_MAG0 0  //File identification
#define EI_MAG1 1  //File identification
#define EI_MAG2 2  //File identification
#define EI_MAG3 3  //File identification
#define EI_CLASS 4  //File class
#define EI_DATA 5  //Data encoding
#define EI_VERSION 6  //File version
#define EI_PAD 7  //Start of padding bytes
#define EI_NIDENT 16  //Size of e_ident[]

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

static uint32_t elf_is_valid(void *p);
int64_t elf_start_offset(void *p);

#endif
