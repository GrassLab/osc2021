#include "../include/syscall.h"
#include "../include/io.h"
#include "../include/string.h"

int main() {
    io() << "Fork Test, pid " << getpid() << "\r\n";
    int cnt = 1;
    int ret = 0;
    if ((ret = fork()) == 0) { // child
        io() << "pid: " << getpid() << ", cnt: " << cnt << ", ptr: " << &cnt << "\r\n";
        ++cnt;
        fork();
        while (cnt < 5) {
            io() << "pid: " << getpid() << ", cnt: " << cnt << ", ptr: " << &cnt << "\r\n";
            delay(10000000);
            ++cnt;
        }
    } else {
        io() << "parent here, pid " << getpid() << ", child " << ret << "\r\n";
    }
}
