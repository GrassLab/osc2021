#include <terminal.h>
#include <mini_uart.h>
#include <string.h>
#include <kernel.h>
#include <cpio.h>

Terminal::Terminal() {
    MiniUART::Init();
}

void Terminal::Run() {
    MiniUART::PutS("Welcome to use Hello Shell.\r\nType \"help\" for more information.\r\n");
    MiniUART::PutS("% ");

    while (true) {
        MiniUART::GetS(buffer);
        bool command_known = false;
        if (String::Equal(buffer, "")) {
            command_known = true;
        }
        else {
            char buffer2[1024];
            int offset = 0;
            while (buffer[offset] != '\0' && buffer[offset] != ' ') {
                buffer2[offset] = buffer[offset];
                offset++;
            }
            buffer2[offset] = '\0';
            for (NameFuncMap& elem : map) {
                if (String::Equal(buffer2, elem.command)) {
                    command_known = true;
                    (this->*elem.func)();
                    break;
                }
            }
        }
        if (!command_known) {
            MiniUART::PutS("Unknown command\r\n");
        }
        MiniUART::PutS("% ");
    }
}

void Terminal::Help() {
    MiniUART::PutS(
        "--------------------------------------\r\n"
        "command | description\r\n"
        "--------+-----------------------------\r\n"
        "help    | print all available commands\r\n"
        "hello   | print Hello World!\r\n"
        "reboot  | reboot the raspi\r\n"
        "ls      | list filenames and file contents in cpio\r\n"
        "malloc  | allocate memory\r\n"
        "free    | free memory\r\n"
        "start   | Start Timer\r\n"
        "stop    | Stop Timer\r\n"
    );
}

void Terminal::Hello() {
    MiniUART::PutS("Hello World!\r\n");
}

void Terminal::Reboot() {
    MiniUART::PutS("Rebooting...\r\n");
    Kernel::Reset(100);
    while (true);
}

void Terminal::Ls() {
    cpio_newc_header* header = reinterpret_cast<cpio_newc_header*>(0x8000000);
    while (true) {
        CPIO cpio(header);
        if (String::Equal(cpio.filename, "TRAILER!!!")) {
            break;
        }
        MiniUART::PutS("Filename: ");
        MiniUART::PutS(cpio.filename, cpio.namesize);
        MiniUART::PutS("\r\n");
        MiniUART::PutS("File content:\r\n");
        MiniUART::PutS(cpio.filecontent, cpio.filesize);
        MiniUART::PutS("\r\n===============================\r\n");
        header = reinterpret_cast<cpio_newc_header*>((reinterpret_cast<uint64_t>(header) + sizeof(cpio_newc_header) + cpio.namesize + cpio.filesize + 3) & ~3);
    }
}

void Terminal::Malloc() {
    char* ptr = buffer;
    while (*ptr != ' ' && *ptr != '\0') ptr++;
    while (*ptr == ' ' && *ptr != '\0') ptr++;
    uint64_t val = 0;
    while (*ptr <= '9' && *ptr >= '0') {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    char* result = memAlloc.malloc(val);
    if (result) {
        IO() << "Allocated memory width address " << uint64_t(result) << "\r\n";
    }
    else {
        IO() << "Failed to allocate memory\r\n";
    }

}
void Terminal::Free() {
    char* ptr = buffer;
    while (*ptr != ' ' && *ptr != '\0') ptr++;
    while (*ptr == ' ' && *ptr != '\0') ptr++;
    uint64_t val = 0;
    while (*ptr <= '9' && *ptr >= '0') {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    bool result = memAlloc.free((char*)val);
    if (result) {
        IO() << "Freed memory " << val << "\r\n";
    }
    else {
        IO() << "Failed to free memory\r\n";
    }
}

void Terminal::TriggerException() {
    asm volatile("svc 1");
}

void Terminal::StartTimer() {
    asm volatile("svc 2");
}

void Terminal::StopTimer() {
    asm volatile("svc 3");
}
