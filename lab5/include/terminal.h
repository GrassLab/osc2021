#ifndef TERMINAL_H
#define TERMINAL_H

#include <memalloc.h>

class Terminal;

struct NameFuncMap {
    const char* command = nullptr;
    void(Terminal::*func)() = nullptr;
};

class Terminal {
public:
    Terminal();
    void Run();
private:
    void Ls();
    void Help();
    void Hello();
    void Reboot();
    void Malloc();
    void Free();
    void StartTimer();
    void StopTimer();
    void TriggerException();
    void TestThread();
    char buffer[1024];
    MemAlloc memAlloc;
    NameFuncMap map[10] = {
        {"ls", &Terminal::Ls},
        {"help", &Terminal::Help},
        {"hello", &Terminal::Hello},
        {"reboot", &Terminal::Reboot},
        {"malloc", &Terminal::Malloc},
        {"free", &Terminal::Free},
        {"start", &Terminal::StartTimer},
        {"stop", &Terminal::StopTimer},
        {"except", &Terminal::TriggerException},
        {"thread", &Terminal::TestThread}
    };
};

#endif
