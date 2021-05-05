#ifndef _ELF_H_
#include <types.h>

#define EI_NIDENT 16

#define ElfN_Addr uint64_t
#define ElfN_Off  uint64_t
#define ElfN_Section uint16_t
#define ElfN_Versym uint16_t
#define Elf_Byte unsigned char
#define ElfN_Half uint16_t
#define ElfN_Sword int32_t
#define ElfN_Word uint32_t
#define ElfN_Sxword int64_t
#define ElfN_Xword uint64_t

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
  ElfN_Addr e_entry;
  ElfN_Off e_phoff;
  ElfN_Off e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
} ElfN_Ehdr;

typedef struct {
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  ElfN_Addr sh_addr;
  ElfN_Off sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
} Elf64_Shdr;

void* elf_header_parse(void* addr);
#endif