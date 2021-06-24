#include <syscall.h>
#include <io.h>
#include <types.h>

int main(void) {
  int cnt = 0;
  char buffer[20];
  int pid = fork();
  if(pid == 0) {
    int pid2 = fork();
    int pid3 = fork();
    while(cnt < 10) {
      io() << "pid: " << getpid() << ", sp: " << u64tohex((uint64_t)&cnt, buffer, sizeof(buffer)) << " cnt: " << (cnt++) << "\r\n"; // address should be the same, but the cnt should be increased indepndently
      delay(1000000);
    }
    if (pid2 != 0) wait(pid2);
    if (pid3 != 0) wait(pid3);
  } else {
    wait(pid);
    int* a = 0x0; // a non-mapped address.
    io() << u64tohex((uint16_t)*a, buffer, sizeof(buffer)) << "\r\n"; // trigger simple page fault.
    io() << "Should not be printed\r\n";
  }
}
