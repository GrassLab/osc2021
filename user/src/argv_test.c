#include "printf.h"
#include "syscall.h"
#include "utils.h"

int main(int argc, char **argv) {
  printf("Argv Test, pid %d\n", getpid());

  for (int i = 0; i < argc; ++i) {
    printf("%s\n", argv[i]);
  }
  char *fork_argv[] = {"fork_test", 0};
  exec("fork_test", fork_argv);

  return 0;
}