#include "../include/types.h"
#include "../include/string.h"

extern "C" {
    void uart_put(const char* str, size_t count);
    void exec(const char* name, const char **argv);
    uint64_t getpid();
}

int main(int argc, char** argv) {
    char buffer[20];
    char *hello = "Hello World\r\n";
    uart_put(hello, strlen(hello));
    // uart_put("argc: ", 6);
    // u64toa(argc, buffer, 20);
    // uart_put(buffer, strlen(buffer));
    // uart_put("\r\n", 2);
    for (int i = 0; i < argc; i++) {
        uart_put(argv[i], strlen(argv[i]));
        uart_put("\r\n", 2);
    }
    uart_put("thread id: ", strlen("thread id: "));
    u64toa(getpid(), buffer, 20);
    uart_put(buffer, strlen(buffer));
    uart_put("\r\n", 2);
    // exec(argv[0], (const char**)argv);
    return 0;
}
