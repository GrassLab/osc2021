#ifndef TERMINAL_H
#define TERMINAL_H

class Terminal {
public:
    Terminal();
    void Run();
private:
    void Help();
    void Hello();
    void Reboot();
    void Ls();
};

#endif
