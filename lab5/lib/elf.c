#include "elf.h"
#include <printf.h>
#include <string.h>

void* elf_header_parse(void* addr) {
  ElfN_Ehdr * elf_header;
  Elf64_Shdr * section_header_arr;
  void* shst;
  int name_idx;
  //elf header address
  elf_header = (ElfN_Ehdr *)addr;
  //section header address
  section_header_arr = (Elf64_Shdr* )(addr + elf_header->e_shoff);
  //section string table address
  shst = addr + ((Elf64_Shdr* )(section_header_arr + elf_header->e_shstrndx))->sh_offset;
  
  //find .text section
  for(int i = 0; i < elf_header->e_shnum; i++) {
    name_idx = (section_header_arr + i)->sh_name;
    if(strncmp((char* )shst + name_idx, ".text", 5) == 0) {
      printf("%s\n", (char* )shst + name_idx);
      return addr + (section_header_arr + i)->sh_offset;
    }
  }
  
  return null;
}
