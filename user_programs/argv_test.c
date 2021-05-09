#include "lib.h"

int main(int argc, char **argv) {
  int pid = getpid();
  printf("Argv Test, pid %d\n", pid);
  for (int i = 0; i < argc; ++i) {
    printf("  argv[%d] = %s\n", i, argv[i]);
  }

  char *fork_argv[2];
  fork_argv[0] = "./fork_test.out";
  fork_argv[1] = NULL;
  exec("./fork_test.out", fork_argv);
  return 0;
}