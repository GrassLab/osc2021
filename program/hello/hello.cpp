#include "../include/syscall.h"
#include "../include/io.h"
#include "../include/string.h"

int main(int argc, char **argv) {
    io() << "Argv Test, pid " << getpid() << "\r\n";
    for (int i = 0; i < argc; ++i) {
        io() << (argv[i]) << "\r\n";
    }
    char *fork_argv[] = {"hello2", 0};
    exec("hello2", (const char**)fork_argv);
}
