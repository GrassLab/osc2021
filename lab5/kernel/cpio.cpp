#include "cpio.h"

CPIO::CPIO(cpio_newc_header* header) {
    this->filesize = 0;
    this->namesize = 0;
    this->header = header;
    for (int i = 0; i < 8; i++) {
        this->filesize = this->filesize * 16 + Hex2int(header->c_filesize[i]);
        this->namesize = this->namesize * 16 + Hex2int(header->c_namesize[i]);
    }
    this->filename = reinterpret_cast<char*>(header) + sizeof(cpio_newc_header);
    this->filecontent = (char*)((uint64_t(this->filename + this->namesize) + 3) & ~3);
}

cpio_newc_header* CPIO::next() {
    cpio_newc_header* ret = (cpio_newc_header*)((uint64_t(header) + sizeof(cpio_newc_header) + namesize + filesize + 3) & ~3);
    return ret;
}

int CPIO::Hex2int(uint8_t hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    return hex - 'A' + 10;
}
