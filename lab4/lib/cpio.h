#ifndef CPIO_H
#define CPIO_H
#include <types.h>
#include <string.h>
#include <uart.h>

#define CPIO_NEWC_HEADER_LENGTH 110
#define CPIO_ADDRESS 0x8000000
#define CPIO_FILESIZE_OFFSET 54
#define CPIO_NAMESIZE_OFFSET 94
#define CPIO_FILE_LIST_SIZE 100
/*struct __attribute__((__packed__)) cpio_newc_header {
    char c_magic[6];
    char c_ino[8];
    char c_mode[8];
    char c_uid[8];
    char c_gid[8];
    char c_nlink[8];
    char c_mtime[8];
    char c_filesize[8];
    char c_devmajor[8];
    char c_devminor[8];
    char c_rdevmajor[8];
    char c_rdevminor[8];
    char c_namesize[8];
    char c_check[8];
};*/

//cpio metadata for pathname address, s__attribute__((aligned(0x10)))ize, and file content address, size
struct cpio_metadata {
    void* header_address;
    void* name_address;
    uint32_t name_size;
    void* file_address;
    uint32_t file_size;
};
//structure array for cpio metadata
struct cpio_metadata cpio_file_list[CPIO_FILE_LIST_SIZE];
//structure array size
uint32_t cpio_file_list_size;

void cpio_parse_newc_header(void* address);
void cpio_get_file_content(char* pathname, uint32_t size);
void cpio_get_all_pathname();
void* cpio_get_file_address(char* pathname, uint32_t size);
void* cpio_load_program(char* filename, size_t size, void* address);
#endif