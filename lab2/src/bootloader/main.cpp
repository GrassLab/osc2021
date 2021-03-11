#include <stdint.h>
#include <mini_uart.h>

void main() {
    union {
        uint8_t program_size_bytes[8];
        uint64_t program_size;
    };
    uint8_t* program_start = reinterpret_cast<uint8_t*>(0x80000);
    MiniUART::Init();
    // Init magic = 0x01020304 in big endian
    while (true) {
        uint8_t data;
        data = MiniUART::Recv();
        if (data != 1) continue;
label_1:
        data = MiniUART::Recv();
        if (data == 1) goto label_1;
        if (data != 2) continue;
        
        data = MiniUART::Recv();
        if (data == 1) goto label_1;
        if (data != 3) continue;
        
        data = MiniUART::Recv();
        if (data == 1) goto label_1;
        if (data != 4) continue;
        break;
    }
    for (auto& byte : program_size_bytes) byte = MiniUART::Recv();
    for (uint64_t offset = 0; offset < program_size; offset ++) program_start[offset] = MiniUART::Recv();
    asm volatile("br %x[program_start]"::[program_start]"r"(program_start));
}
