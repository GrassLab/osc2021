#include <kernel/cpio.h>
#include <kernel/string.h>

CPIO::CPIO(cpio_newc_header* header) {
    this->filesize = 0;
    this->namesize = 0;
    this->header = header;
    char buffer[30];
    for (int i = 0; i < 8; i++) {
        filesize = (filesize << 4) + Hex2int(header->c_filesize[i]);
        namesize = (namesize << 4) + Hex2int(header->c_namesize[i]);
    }
    this->filename = reinterpret_cast<char*>(header) + sizeof(cpio_newc_header);
    this->filecontent = (char*)((uint64_t(this->filename + this->namesize) + 3) & ~3);
    this->next = (cpio_newc_header*)((uint64_t(this->filecontent + this->filesize) + 3) & ~3);
}

int CPIO::Hex2int(uint8_t hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    return hex - 'A' + 10;
}
