#include "lib.h"
int main() {

  char *name = "./argv_test.out";
  char *args[4];
  args[0] = "./argv_test.out";
  args[1] = "-o";
  args[2] = "arg2";
  args[3] = NULL;
  exec(name, (const char **)args);
  return 0;
}

// int main(int argc, char **argv) {
// int pid = getpid();
// printf("Argv Test, pid %d\n", pid);
// for (int i = 0; i < argc; ++i) {
//   printf("  argv[%d] = %s\n", i, argv[i]);
// }
// char *fork_argv[] = {"fork_test", 0};
// exec("fork_test", fork_argv);
// }