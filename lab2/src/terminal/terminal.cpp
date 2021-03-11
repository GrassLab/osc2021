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

    char buffer[1024];
    while (true) {
        MiniUART::GetS(buffer);
        if (String::Equal(buffer, ""));
        else if (String::Equal(buffer, "help")) {
            Help();
        }
        else if (String::Equal(buffer, "hello")) {
            Hello();
        }
        else if (String::Equal(buffer, "reboot")) {
            Reboot();
        }
        else if (String::Equal(buffer, "ls")) {
            Ls();
        }
        else {
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
