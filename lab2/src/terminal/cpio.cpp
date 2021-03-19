#include <cpio.h>

CPIO::CPIO(cpio_newc_header* header) {
    filesize = 0;
    namesize = 0;
    for (int i = 0; i < 8; i++) {
        filesize = (filesize << 4) + Hex2int(header->c_filesize[i]);
        namesize = (namesize << 4) + Hex2int(header->c_namesize[i]);
    }
    filename = reinterpret_cast<char*>(header) + sizeof(cpio_newc_header);
    filecontent = Align4(filename + namesize);
    next = reinterpret_cast<cpio_newc_header*>(Align4(filecontent + filesize));
}

int CPIO::Hex2int(uint8_t hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    return hex - 'A' + 10;
}

char* CPIO::Align4(char* ptr) {
    return reinterpret_cast<char*>((reinterpret_cast<uint64_t>(ptr) + 3) & ~3);
}
