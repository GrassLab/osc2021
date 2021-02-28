#include <terminal.h>
#include <mini_uart.h>
#include <string.h>
#include <kernel.h>

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
    );
}

void Terminal::Hello() {
    MiniUART::PutS("Hello World!\r\n");
}

void Terminal::Reboot() {
    Kernel::Reset(100);
}
