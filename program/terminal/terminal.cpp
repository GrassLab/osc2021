#include <io.h>
#include <string.h>

class Terminal;
struct NameFuncMap {
    const char* command = nullptr;
    void(Terminal::*func)() = nullptr;
};

class Terminal {
public:
    void Run() {
        io() << "Welcome to use Shell.\r\nType \"help\" for more information.\r\n% ";
        while (true) {
            char* argv[16];
            int size = uart_get(buffer, sizeof(buffer));
            buffer[size] = 0;
            char *token = strtok(buffer, " ");
            int i = 0;
            while (token != nullptr) {
                if (strcmp(token, "") != 0) {
                    argv[i] = token;
                    i++;
                }
                token = strtok(nullptr, " ");
            }
            argv[i] = nullptr;
            bool known_command = false;
            for (NameFuncMap& elem : map) {
                if (strcmp(elem.command, argv[0]) == 0) {
                    (this->*elem.func)();
                    known_command = true;
                    break;
                }
            }
            if (argv[0] != nullptr && !known_command) {
                int pid = fork();
                if (pid == 0) {
                    exec(argv[0], (const char**)argv);
                    io() << "Unknown command\r\n";
                    exit();
                }
                wait(pid);
            }
            io() << "% ";
        }
    }
private:
    char buffer[256];
    NameFuncMap map[2] = {
        {"help", &Terminal::Help},
        {"exit", &Terminal::Exit}
    };

    void Help() {
        io() << "--------------------------------------\r\n"
                "command | description\r\n"
                "--------+-----------------------------\r\n"
                "help    | print all available commands\r\n";
    }

    void Exit() {
        exit();
    }
};

int main() {
    Terminal().Run();
}
