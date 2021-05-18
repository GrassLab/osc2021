#include "start.h"

int main(int argc, char **argv) {
  // printf("Argv Test, pid %d\n", getpid());
  print_s("Argv Test, pid ");
  print_i(getpid());
  print_s("\n");
  for (int i = 0; i < argc; ++i) {
    // puts(argv[i]);
    print_s(argv[i]);
    print_s("\n");
  }
  char *fork_argv[] = {"fork_test", 0};
  exec("fork_test", fork_argv);

  return 0;
}