#include <stdint.h>
#include <mini_uart.h>

void main(void* dtd_addr) {
    union {
        uint8_t program_size_bytes[8];
        uint64_t program_size;
    };
    union {
        void(*program_start)(void*);
        uint8_t* program_bytes;
        uint64_t _ = 0x80000;
    };
    
    MiniUART::Init();
    // Init magic = 0x01020304 in big endian
    uint8_t magic_state = 0;
    while (true) {
        uint8_t data = MiniUART::Recv();
        if (magic_state == 0 && data == 1)
            magic_state = data;
        else {
            if (data == 1) magic_state = 1;
            else if (data != magic_state + 1) magic_state = 0;
            else magic_state = data;
            if (magic_state == 4) break;
        }
    }
    for (auto& byte : program_size_bytes) byte = MiniUART::Recv();
    for (uint64_t offset = 0; offset < program_size; offset++) program_bytes[offset] = MiniUART::Recv();
    program_start(dtd_addr);
}
