#include "cpio.h"
#include <string.h>

void cpio_parse_newc_header(void* address) {
  int i = 0;
  char buf[9];
  uint32_t name_size, file_size;
  cpio_file_list_size = 0;
 
  while(1) {
    
    cpio_file_list[i].header_address = address;

    //get name size
    strncpy(buf, (char *)(address + CPIO_NAMESIZE_OFFSET), 8);
    buf[8] = '\0';
    name_size = strtol(buf, null, 16);
    cpio_file_list[i].name_size = name_size;

    //get file size
    strncpy(buf, (char *)(address + CPIO_FILESIZE_OFFSET), 8);
    buf[8] = '\0';
    file_size = strtol(buf, null, 16);
    cpio_file_list[i].file_size = file_size;
    
    //first header or directory
    if(file_size == 0) {
      //empty file
      address += CPIO_NEWC_HEADER_LENGTH;
      
       //cpio end
      if(name_size >= 11 && strncmp((char *)address, "TRAILER!!!", 10) == 0) {
        cpio_file_list_size = i;
        break;
      }
      address += name_size;

       /* padding */
      if((CPIO_NEWC_HEADER_LENGTH + name_size) % 4 != 0)
        address += 4 - (CPIO_NEWC_HEADER_LENGTH + name_size) % 4;

      continue;
    }

    address += CPIO_NEWC_HEADER_LENGTH;

    //get name address
    cpio_file_list[i].name_address = address;
    address += name_size;

    /* padding */
    if((CPIO_NEWC_HEADER_LENGTH + name_size) % 4 != 0)
      address += 4 - (CPIO_NEWC_HEADER_LENGTH + name_size) % 4;

    //get content address
    cpio_file_list[i].file_address = address;
    address += file_size;

    /* padding */
    if(file_size % 4 != 0)
      address += 4 - (file_size % 4);
    i++;
  }
}

void cpio_get_file_content(char* pathname, uint32_t size) {
  for(int i = 0; i < cpio_file_list_size; i++) {
    if(strncmp((char *)cpio_file_list[i].name_address, pathname, size) == 0) {
      do_uart_write((char *)cpio_file_list[i].file_address, cpio_file_list[i].file_size);
      uart_puts("\n");
      return;
    }
  }
  uart_puts("file not found.\n");
}

void cpio_get_all_pathname() {
  for(int i = 0; i < cpio_file_list_size; i++) {
    do_uart_write((char *)cpio_file_list[i].name_address, cpio_file_list[i].name_size);
    uart_send(' ');
  }
  uart_puts("\n");
}

void* cpio_get_file_address(char* pathname, uint32_t size) {
  for(int i = 0; i < cpio_file_list_size; i++) {
    if(strncmp((char *)cpio_file_list[i].name_address, pathname, size) == 0) {
      return (void* )cpio_file_list[i].file_address;
    }
  }
  return null;
}

void* cpio_get_metadata(const char* pathname, uint32_t size) {
  for(int i = 0; i < cpio_file_list_size; i++) {
    if(strncmp((char *)cpio_file_list[i].name_address, pathname, cpio_file_list[i].name_size) == 0) {
      return (void* )&cpio_file_list[i];
    }
  }
  return null;
}
void* cpio_load_program(char* filename, size_t size, void* address) {
  struct cpio_metadata *metadata;

  metadata = (struct cpio_metadata *)cpio_get_metadata(filename, size);
  
  //should check overlap
  
  if(metadata == null)
    return null;
  printf("addr: %x\n", metadata->file_address);
  //move program to specific address
  memcpy((char *)address, (char *)metadata->file_address, metadata->file_size);
  return address;
}