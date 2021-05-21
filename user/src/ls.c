#include "start.h"

int main(int argc, char** argv) {
  print_s("Directory: ");
  print_s(argv[1]);
  print_s("\n");

  int fd = open(argv[1], 0);
  char name[100];
  int size;
  for (int i = 0;; ++i) {
    size = list(fd, name, i);
    if (size < 0) break;
    if (size > 0) {
      print_s("Name: ");
      print_s(name);
      print_s(", size: ");
      print_i(size);
      print_s("\n");
    }
  }

  return 0;
}