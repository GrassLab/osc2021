#include <syscall.h>
#include <printf.h>
int main() {

  printf("user_main\n");
  printf("pid: %d\n", getpid());
  exit(0);
  
  return 0;
}