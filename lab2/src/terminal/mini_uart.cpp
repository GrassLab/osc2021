#include <mini_uart.h>

#include <mmio.h>
#include <kernel.h>

void MiniUART::Init() {
    uint32_t gpfsel1_value = MMIO::get(MMIOREG::GPFSEL1);
    gpfsel1_value = gpfsel1_value & ~((7 << 12) | (7 << 15)) | ((2 << 12) | (2 << 15));
    MMIO::set(MMIOREG::GPFSEL1, gpfsel1_value);
    MMIO::set(MMIOREG::GPPUD, 0);
    Kernel::Delay(150);
    MMIO::set(MMIOREG::GPPUDCLK0, (1 << 14) | (1 << 15));
    Kernel::Delay(150);
    MMIO::set(MMIOREG::GPPUD, 0);
    MMIO::set(MMIOREG::GPPUDCLK0, 0);

    MMIO::set(MMIOREG::AUX_ENABLES, 1);
    MMIO::set(MMIOREG::AUX_MU_CNTL_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_IER_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_LCR_REG, 3);
    MMIO::set(MMIOREG::AUX_MU_MCR_REG, 0);
    MMIO::set(MMIOREG::AUX_MU_BAUD_REG, 270);
    MMIO::set(MMIOREG::AUX_MU_IIR_REG, 6);
    MMIO::set(MMIOREG::AUX_MU_CNTL_REG, 3);
}

void MiniUART::Send(uint8_t ch) {
    while(!(MMIO::get(MMIOREG::AUX_MU_LSR_REG) & 0x20));
    MMIO::set(MMIOREG::AUX_MU_IO_REG, ch);
}

uint8_t MiniUART::Recv() {
    while(!(MMIO::get(MMIOREG::AUX_MU_LSR_REG) & 1));
    return MMIO::get(MMIOREG::AUX_MU_IO_REG);
}

uint8_t MiniUART::GetCh() {
    uint8_t ch = Recv();
    if (ch == '\b' || ch == 127) {
        ch = '\b';
    }
    else if (ch == '\r') {
        Send('\r');
        Send('\n');
    }
    else if (ch < 32) {
        Send('^');
        Send(ch + 64);
    }
    else if (ch > 127) {
        Send('\\');
        Send('x');
        uint8_t ch1 = (ch >> 4) & 0xf, ch2 = ch & 0xf;
        ch1 = (ch1 < 10) ? ch1 + '0' : ch1 - 10 + 'A';
        ch2 = (ch2 < 10) ? ch2 + '0' : ch2 - 10 + 'A';
        Send(ch1);
        Send(ch2);
    }
    else {
        Send(ch);
    }
    return ch;
}

void MiniUART::GetS(char* str) {
    uint32_t offset = 0;
    while (true) {
        uint8_t ch = GetCh();
        if (ch == '\r') {
            str[offset] = 0;
            return;
        }
        else if (ch == '\b') {
            if (offset > 0) {
                offset--;
                if (str[offset] < 32) {
                    PutS("\b\b  \b\b");
                }
                else if (str[offset] > 127) {
                    PutS("\b\b\b\b    \b\b\b\b");
                }
                else {
                    PutS("\b \b");
                }
            }
        }
        else {
            str[offset++] = ch;
        }
    }
}

void MiniUART::PutS(const char* str) {
    while (*str != 0) {
        Send(*str);
        str++;
    }
}

void MiniUART::PutS(const char* str, uint64_t length) {
    uint64_t offset = 0;
    while (offset < length) {
        Send(str[offset]);
        offset++;
    }
}
