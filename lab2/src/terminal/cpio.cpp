#include <cpio.h>

CPIO::CPIO(cpio_newc_header* header) {
    this->filesize = 0;
    this->namesize = 0;
    for (int i = 0; i < 8; i++) {
        this->filesize = this->filesize * 16 + Hex2int(header->c_filesize[i]);
        this->namesize = this->namesize * 16 + Hex2int(header->c_namesize[i]);
    }
    this->filename = reinterpret_cast<char*>(header) + sizeof(cpio_newc_header);
    this->filecontent = this->filename + this->namesize;
}

int CPIO::Hex2int(uint8_t hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    return hex - 'A' + 10;
}
