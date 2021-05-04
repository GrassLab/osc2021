#include "lib.h"

int main(int argc, char **argv) {
  int pid = getpid();
  printf("Argv Test, pid %d\n", pid);
  for (int i = 0; i < argc; ++i) {
    printf("  argv[%d] = %s\n", i, argv[i]);
  }
  return 0;
  // char *fork_argv[] = {"fork_test", 0};
  // exec("fork_test", fork_argv);
}