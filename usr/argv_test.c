#include "start.h"

void main(int argc, char **argv) {
  print("Argv Test, pid ");
  print_hex(get_pid());
  print("\n");
  for (int i = 0; i < argc; ++i) {
    puts(argv[i]);
  }
  char *fork_argv[] = {"fork_test.img", 0};
  execve("fork_test.img", fork_argv);
}