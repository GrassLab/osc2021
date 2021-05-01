#include "start.h"

void main() {
  print("Fork Test, pid ");
  print_hex(get_pid());
  print("\n");
  int cnt = 1;
  int ret = 0;
  if ((ret = fork()) == 0) {
    print("pid: ");
    print_hex(get_pid());
    print(", cnt: ");
    print_hex(cnt);
    print(", ptr: ");
    print_hex((unsigned long)&cnt);
    print("\n");
    ++cnt;
    fork();
    while (cnt < 5) {
      print("pid: ");
      print_hex(get_pid());
      print(", cnt: ");
      print_hex(cnt);
      print(", ptr: ");
      print_hex((unsigned long)&cnt);
      print("\n");
      delay(1000000);
      ++cnt;
    }
  } else {
    print("parent here, pid ");
    print_hex(get_pid());
    print(", child ");
    print_hex(ret);
    print("\n");
  }
}